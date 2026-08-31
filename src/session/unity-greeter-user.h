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

G_BEGIN_DECLS

/* Visible, unlocked accounts, most frequent first. Holds #ActUser. */
GListModel *unity_greeter_users_new (void);

/* Real name when set, account name otherwise. */
const gchar *unity_greeter_user_display_name (ActUser *user);

/* AccountsService SetSession, asynchronous. */
void unity_greeter_user_persist_session (ActUser     *user,
                                         const gchar *session_id);

/* Writes @password to shadow through AccountsService SetPassword. Clears
   the SET_AT_LOGIN marker, so the next PAM acct_mgmt asks for nothing.
   Synchronous D-Bus call. */
void unity_greeter_user_set_password (ActUser     *user,
                                      const gchar *password);

G_END_DECLS
