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

#include <libdex.h>

const gchar *
unity_greeter_user_display_name (ActUser *user)
{
  const gchar *real = act_user_get_real_name (user);
  if (real != NULL && *real != '\0')
    return real;
  return act_user_get_user_name (user);
}

static DexFuture *
set_session_fiber (gpointer data)
{
  g_autoptr (GVariant) params = data;
  const gchar *object_path;
  const gchar *session_id;

  g_variant_get (params, "(&s&s)", &object_path, &session_id);

  g_autoptr (GError) error = NULL;
  g_autoptr (GDBusConnection) bus = dex_await_object (
    dex_bus_get (G_BUS_TYPE_SYSTEM), &error);
  if (bus == NULL)
    {
      g_warning ("SetSession: no system bus: %s", error->message);
      return dex_future_new_true ();
    }

  g_autoptr (GVariant) reply = dex_await_variant (
    dex_dbus_connection_call (bus,
                              "org.freedesktop.Accounts",
                              object_path,
                              "org.freedesktop.Accounts.User",
                              "SetSession",
                              g_variant_new ("(s)", session_id),
                              NULL, G_DBUS_CALL_FLAGS_NONE, -1),
    &error);
  if (reply == NULL)
    g_warning ("SetSession: %s", error->message);

  return dex_future_new_true ();
}

void
unity_greeter_user_persist_session (ActUser *user, const gchar *session_id)
{
  g_return_if_fail (ACT_IS_USER (user));
  g_return_if_fail (session_id != NULL);

  const gchar *object_path = act_user_get_object_path (user);
  if (object_path == NULL)
    return;

  GVariant *params = g_variant_ref_sink (
    g_variant_new ("(ss)", object_path, session_id));

  dex_future_disown (dex_scheduler_spawn (
    NULL, 0, set_session_fiber, params, (GDestroyNotify) g_variant_unref));
}

void
unity_greeter_user_set_password (ActUser *user, const gchar *password)
{
  g_return_if_fail (ACT_IS_USER (user));
  g_return_if_fail (password != NULL);

  act_user_set_password (user, password, "");
}

static gint
compare_by_login_frequency (gconstpointer a, gconstpointer b, gpointer user_data)
{
  gint fa = act_user_get_login_frequency ((ActUser *) a);
  gint fb = act_user_get_login_frequency ((ActUser *) b);
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

      g_list_store_append (store, user);
    }
  g_slist_free (users);

  g_list_store_sort (store, compare_by_login_frequency, NULL);
}

static void
on_manager_notify_loaded (GObject *object, GParamSpec *pspec, gpointer store_ptr)
{
  gboolean is_loaded = FALSE;
  g_object_get (object, "is-loaded", &is_loaded, NULL);
  if (is_loaded)
    populate_from_manager (G_LIST_STORE (store_ptr), ACT_USER_MANAGER (object));
}

GListModel *
unity_greeter_users_new (void)
{
  GListStore *store = g_list_store_new (ACT_TYPE_USER);
  ActUserManager *manager = g_object_ref (act_user_manager_get_default ());

  /* The store owns the manager: get_default is (transfer none). */
  g_object_set_data_full (G_OBJECT (store), "act-manager",
                          manager, g_object_unref);

  g_signal_connect_object (manager, "notify::is-loaded",
                           G_CALLBACK (on_manager_notify_loaded), store, G_CONNECT_DEFAULT);

  on_manager_notify_loaded (G_OBJECT (manager), NULL, store);

  return G_LIST_MODEL (store);
}
