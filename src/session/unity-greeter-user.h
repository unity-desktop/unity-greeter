/* unity-greeter-user.h
 *
 * Copyright 2026 Muqtadir
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
