/* unity-greeter-session-list.c
 *
 * Copyright 2026 Muqtadir
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "unity-greeter-session-list.h"

#include <string.h>

#include <gio/gdesktopappinfo.h>

struct _UnityGreeterSession
{
  GObject parent_instance;

  gchar *id;
  gchar *name;
  gchar *comment;
  gchar *command;
  gchar *desktop_names;
};

G_DEFINE_FINAL_TYPE (UnityGreeterSession, unity_greeter_session, G_TYPE_OBJECT)

static void
unity_greeter_session_finalize (GObject *object)
{
  UnityGreeterSession *self = UNITY_GREETER_SESSION (object);

  g_clear_pointer (&self->id,            g_free);
  g_clear_pointer (&self->name,          g_free);
  g_clear_pointer (&self->comment,       g_free);
  g_clear_pointer (&self->command,       g_free);
  g_clear_pointer (&self->desktop_names, g_free);

  G_OBJECT_CLASS (unity_greeter_session_parent_class)->finalize (object);
}

static void
unity_greeter_session_class_init (UnityGreeterSessionClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = unity_greeter_session_finalize;
}

static void
unity_greeter_session_init (UnityGreeterSession *self)
{
}

const gchar *unity_greeter_session_get_id            (UnityGreeterSession *self) { return self->id; }
const gchar *unity_greeter_session_get_name          (UnityGreeterSession *self) { return self->name; }
const gchar *unity_greeter_session_get_comment       (UnityGreeterSession *self) { return self->comment; }
const gchar *unity_greeter_session_get_desktop_names (UnityGreeterSession *self) { return self->desktop_names; }

GStrv
unity_greeter_session_parse_command (UnityGreeterSession *self, GError **error)
{
  g_return_val_if_fail (UNITY_GREETER_IS_SESSION (self), NULL);

  gint    argc = 0;
  gchar **argv = NULL;
  if (!g_shell_parse_argv (self->command, &argc, &argv, error))
    return NULL;
  return argv;
}

GStrv
unity_greeter_session_build_environment (UnityGreeterSession *self)
{
  g_return_val_if_fail (UNITY_GREETER_IS_SESSION (self), NULL);

  GPtrArray *env = g_ptr_array_new_with_free_func (g_free);
  g_ptr_array_add (env, g_strdup ("XDG_SESSION_TYPE=wayland"));
  g_ptr_array_add (env, g_strdup_printf ("XDG_SESSION_DESKTOP=%s", self->id));
  g_ptr_array_add (env, g_strdup_printf ("DESKTOP_SESSION=%s",     self->id));
  if (self->desktop_names != NULL)
    g_ptr_array_add (env,
      g_strdup_printf ("XDG_CURRENT_DESKTOP=%s", self->desktop_names));
  g_ptr_array_add (env, NULL);

  return (GStrv) g_ptr_array_free (env, FALSE);
}

static UnityGreeterSession *
session_from_desktop_file (const gchar *path, const gchar *filename)
{
  g_autoptr (GDesktopAppInfo) info = g_desktop_app_info_new_from_filename (path);
  if (info == NULL)
    return NULL;
  if (g_desktop_app_info_get_is_hidden (info))
    return NULL;
  if (g_desktop_app_info_get_nodisplay (info))
    return NULL;

  const gchar *command = g_app_info_get_commandline (G_APP_INFO (info));
  if (command == NULL || *command == '\0')
    return NULL;

  UnityGreeterSession *self = g_object_new (UNITY_GREETER_TYPE_SESSION, NULL);

  /* scan_dir_into only offers names that end in .desktop. */
  self->id = g_strndup (filename, strlen (filename) - strlen (".desktop"));

  const gchar *name = g_app_info_get_display_name (G_APP_INFO (info));
  self->name = (name != NULL && *name != '\0') ? g_strdup (name) : g_strdup (self->id);

  const gchar *comment = g_app_info_get_description (G_APP_INFO (info));
  if (comment != NULL && *comment != '\0')
    self->comment = g_strdup (comment);

  self->command = g_strdup (command);

  g_auto (GStrv) names =
    g_desktop_app_info_get_string_list (info, "DesktopNames", NULL);
  if (names != NULL && names[0] != NULL)
    self->desktop_names = g_strjoinv (":", names);

  return self;
}

static gint
compare_by_name (gconstpointer a, gconstpointer b, gpointer user_data)
{
  UnityGreeterSession *ua = UNITY_GREETER_SESSION ((gpointer) a);
  UnityGreeterSession *ub = UNITY_GREETER_SESSION ((gpointer) b);
  return g_strcmp0 (ua->name, ub->name);
}

static void
scan_dir_into (GListStore  *store,
               GHashTable  *seen_ids,
               const gchar *dirpath)
{
  g_autoptr (GDir) dir = g_dir_open (dirpath, 0, NULL);
  if (dir == NULL)
    return;

  const gchar *filename;
  while ((filename = g_dir_read_name (dir)) != NULL)
    {
      if (!g_str_has_suffix (filename, ".desktop"))
        continue;

      g_autofree gchar *path = g_build_filename (dirpath, filename, NULL);

      g_autoptr (UnityGreeterSession) session =
        session_from_desktop_file (path, filename);
      if (session == NULL)
        continue;

      if (g_hash_table_contains (seen_ids, session->id))
        continue;

      g_hash_table_add (seen_ids, g_strdup (session->id));
      g_list_store_append (store, session);
    }
}

GListModel *
unity_greeter_session_list_new (void)
{
  g_autoptr (GListStore) store =
    g_list_store_new (UNITY_GREETER_TYPE_SESSION);
  g_autoptr (GHashTable) seen =
    g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

  const gchar *const *data_dirs = g_get_system_data_dirs ();
  for (const gchar *const *d = data_dirs; d != NULL && *d != NULL; d++)
    {
      g_autofree gchar *dirpath = g_build_filename (*d, "wayland-sessions", NULL);
      scan_dir_into (store, seen, dirpath);
    }

  g_list_store_sort (store, compare_by_name, NULL);

  return G_LIST_MODEL (g_steal_pointer (&store));
}

UnityGreeterSession *
unity_greeter_session_list_find (GListModel *model, const gchar *id)
{
  g_return_val_if_fail (G_IS_LIST_MODEL (model), NULL);
  g_return_val_if_fail (id != NULL, NULL);

  guint n = g_list_model_get_n_items (model);
  for (guint i = 0; i < n; i++)
    {
      /* The model keeps a reference, so the match stays valid. */
      g_autoptr (UnityGreeterSession) session =
        UNITY_GREETER_SESSION (g_list_model_get_item (model, i));
      if (g_strcmp0 (session->id, id) == 0)
        return session;
    }
  return NULL;
}
