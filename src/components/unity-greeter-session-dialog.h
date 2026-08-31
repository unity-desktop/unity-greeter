/* unity-greeter-session-dialog.h
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

#include <adwaita.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_SESSION_DIALOG (unity_greeter_session_dialog_get_type ())

/* Session picker. Emits "session-selected" with the chosen session id. */
G_DECLARE_FINAL_TYPE (UnityGreeterSessionDialog, unity_greeter_session_dialog,
                      UNITY_GREETER, SESSION_DIALOG, AdwDialog)

AdwDialog *unity_greeter_session_dialog_new (GListModel  *sessions,
                                             const gchar *selected_id);

G_END_DECLS
