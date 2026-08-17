/* unity-greeter-user.h
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

#pragma once

#include <act/act.h>
#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

/**
 * UNITY_GREETER_TYPE_USER:
 *
 * The #GType for #UnityGreeterUser.
 */
#define UNITY_GREETER_TYPE_USER (unity_greeter_user_get_type ())

/**
 * UnityGreeterUser:
 *
 * Wrapper around #ActUser with greeter-specific helpers.
 */
G_DECLARE_FINAL_TYPE (UnityGreeterUser, unity_greeter_user,
                      UNITY_GREETER, USER, GObject)

/**
 * unity_greeter_user_new:
 * @user: an #ActUser.
 *
 * Wraps an AccountsService user for use in the greeter.
 *
 * Returns: (transfer full): a new #UnityGreeterUser.
 */
UnityGreeterUser *unity_greeter_user_new (ActUser *user);

/**
 * unity_greeter_users_new:
 *
 * Creates a list model containing visible greeter users.
 *
 * Returns: (transfer full): a #GListModel of #UnityGreeterUser objects.
 */
GListModel *unity_greeter_users_new (void);

/**
 * unity_greeter_user_persist_session:
 * @self: a #UnityGreeterUser.
 * @session_id: id of the chosen session.
 *
 * Persists the selected session id for the user.
 */
void unity_greeter_user_persist_session (UnityGreeterUser *self,
                                         const gchar      *session_id);

/**
 * unity_greeter_user_set_password:
 * @self: a #UnityGreeterUser.
 * @password: the new plaintext password.
 *
 * Writes @password to shadow through AccountsService's SetPassword. This
 * also clears the SET_AT_LOGIN marker, so the next PAM acct_mgmt returns
 * without asking for a chauthtok round. Synchronous D-Bus call.
 */
void unity_greeter_user_set_password    (UnityGreeterUser *self,
                                         const gchar      *password);

/**
 * unity_greeter_user_get_user_name:
 * @self: a #UnityGreeterUser.
 *
 * Gets the account user name.
 *
 * Returns: (transfer none): the user name.
 */
const gchar *unity_greeter_user_get_user_name       (UnityGreeterUser *self);

/**
 * unity_greeter_user_get_display_name:
 * @self: a #UnityGreeterUser.
 *
 * Gets the display name shown in the greeter.
 *
 * Returns: (transfer none): display name text.
 */
const gchar *unity_greeter_user_get_display_name    (UnityGreeterUser *self);

/**
 * unity_greeter_user_get_icon_file:
 * @self: a #UnityGreeterUser.
 *
 * Gets the avatar image file path.
 *
 * Returns: (nullable) (transfer none): icon path.
 */
const gchar *unity_greeter_user_get_icon_file       (UnityGreeterUser *self);

/**
 * unity_greeter_user_get_login_frequency:
 * @self: a #UnityGreeterUser.
 *
 * Gets the AccountsService login frequency value.
 *
 * Returns: the login frequency.
 */
gint         unity_greeter_user_get_login_frequency (UnityGreeterUser *self);

/**
 * unity_greeter_user_get_password_mode:
 * @self: a #UnityGreeterUser.
 *
 * Gets the password mode value from AccountsService.
 *
 * Returns: the password mode.
 */
gint         unity_greeter_user_get_password_mode   (UnityGreeterUser *self);

/**
 * unity_greeter_user_get_session:
 * @self: a #UnityGreeterUser.
 *
 * Gets the user's persisted session id.
 *
 * Returns: (nullable) (transfer none): session id.
 */
const gchar *unity_greeter_user_get_session         (UnityGreeterUser *self);

G_END_DECLS
