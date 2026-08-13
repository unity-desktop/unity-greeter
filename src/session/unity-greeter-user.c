/* unity-greeter-user.c
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

#include "unity-greeter-user.h"

#define ACCOUNTS_SERVICE_BUS_NAME "org.freedesktop.Accounts"

struct _UnityGreeterUser
{
  GObject parent_instance;
  ActUser *act_user;
};

G_DEFINE_FINAL_TYPE (UnityGreeterUser, unity_greeter_user, G_TYPE_OBJECT)

static void
unity_greeter_user_dispose (GObject *object)
{
  UnityGreeterUser *self = UNITY_GREETER_USER (object);
  g_clear_object (&self->act_user);
  G_OBJECT_CLASS (unity_greeter_user_parent_class)->dispose (object);
}

static void
unity_greeter_user_class_init (UnityGreeterUserClass *klass)
{
  G_OBJECT_CLASS (klass)->dispose = unity_greeter_user_dispose;
}

static void
unity_greeter_user_init (UnityGreeterUser *self)
{
  (void) self;
}

UnityGreeterUser *
unity_greeter_user_new (ActUser *user)
{
  g_return_val_if_fail (ACT_IS_USER (user), NULL);
  UnityGreeterUser *self = g_object_new (UNITY_GREETER_TYPE_USER, NULL);
  self->act_user = g_object_ref (user);
  return self;
}

const gchar *unity_greeter_user_get_user_name       (UnityGreeterUser *self) { return act_user_get_user_name (self->act_user); }
const gchar *unity_greeter_user_get_icon_file       (UnityGreeterUser *self) { return act_user_get_icon_file (self->act_user); }
gint         unity_greeter_user_get_login_frequency (UnityGreeterUser *self) { return act_user_get_login_frequency (self->act_user); }
gint         unity_greeter_user_get_password_mode   (UnityGreeterUser *self) { return (gint) act_user_get_password_mode (self->act_user); }
const gchar *unity_greeter_user_get_session         (UnityGreeterUser *self) { return act_user_get_session (self->act_user); }

const gchar *
unity_greeter_user_get_display_name (UnityGreeterUser *self)
{
  const gchar *real = act_user_get_real_name (self->act_user);
  if (real != NULL && *real != '\0')
    return real;
  return act_user_get_user_name (self->act_user);
}

static void
on_set_session_done (GObject *source, GAsyncResult *result, gpointer user_data)
{
  (void) user_data;
  g_autoptr (GError) error = NULL;
  g_autoptr (GVariant) reply =
    g_dbus_connection_call_finish (G_DBUS_CONNECTION (source), result, &error);
  if (reply == NULL)
    g_warning ("SetSession: %s", error->message);
}

void
unity_greeter_user_persist_session (UnityGreeterUser *self, const gchar *session_id)
{
  g_return_if_fail (UNITY_GREETER_IS_USER (self));
  g_return_if_fail (session_id != NULL);

  const gchar *path = act_user_get_object_path (self->act_user);
  if (path == NULL)
    return;

  g_autoptr (GError) error = NULL;
  g_autoptr (GDBusConnection) bus =
    g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, &error);
  if (bus == NULL)
    {
      g_warning ("SetSession: no system bus: %s", error->message);
      return;
    }

  g_dbus_connection_call (bus,
                          ACCOUNTS_SERVICE_BUS_NAME,
                          path,
                          "org.freedesktop.Accounts.User",
                          "SetSession",
                          g_variant_new ("(s)", session_id),
                          NULL,
                          G_DBUS_CALL_FLAGS_NONE,
                          -1, NULL, on_set_session_done, NULL);
}

static gint
compare_by_login_frequency (gconstpointer a, gconstpointer b, gpointer user_data)
{
  (void) user_data;
  gint fa = unity_greeter_user_get_login_frequency ((UnityGreeterUser *) a);
  gint fb = unity_greeter_user_get_login_frequency ((UnityGreeterUser *) b);
  return fb - fa;
}

static void
populate_from_manager (GListStore *store, ActUserManager *manager)
{
  g_list_store_remove_all (store);

  GSList *users = act_user_manager_list_users (manager);
  for (GSList *l = users; l != NULL; l = l->next)
    {
      ActUser *user = ACT_USER (l->data);
      if (act_user_is_system_account (user))
        continue;
      if (act_user_get_locked (user))
        continue;

      g_autoptr (UnityGreeterUser) wrapped = unity_greeter_user_new (user);
      g_list_store_append (store, wrapped);
    }
  g_slist_free (users);

  g_list_store_sort (store, compare_by_login_frequency, NULL);
}

static void
on_manager_notify_loaded (GObject *object, GParamSpec *pspec, gpointer store_ptr)
{
  (void) pspec;
  gboolean is_loaded = FALSE;
  g_object_get (object, "is-loaded", &is_loaded, NULL);
  if (is_loaded)
    populate_from_manager (G_LIST_STORE (store_ptr), ACT_USER_MANAGER (object));
}

GListModel *
unity_greeter_users_new (void)
{
  GListStore *store = g_list_store_new (UNITY_GREETER_TYPE_USER);
  ActUserManager *manager = g_object_ref (act_user_manager_get_default ());

  g_object_set_data_full (G_OBJECT (store), "act-manager",
                          manager, g_object_unref);

  g_signal_connect_object (manager, "notify::is-loaded",
                           G_CALLBACK (on_manager_notify_loaded), store, G_CONNECT_DEFAULT);

  on_manager_notify_loaded (G_OBJECT (manager), NULL, store);

  return G_LIST_MODEL (store);
}
