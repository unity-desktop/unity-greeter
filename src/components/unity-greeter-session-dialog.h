/* unity-greeter-session-dialog.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <gio/gio.h>
#include <unity-dialog-popup.h>

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_SESSION_DIALOG (unity_greeter_session_dialog_get_type ())

/* Session picker. Emits "session-selected" with the chosen session id.
 * Binds its application and monitor to @parent's on construction. */
G_DECLARE_FINAL_TYPE (UnityGreeterSessionDialog, unity_greeter_session_dialog,
                      UNITY_GREETER, SESSION_DIALOG, UnityDialogPopup)

UnityDialogPopup *unity_greeter_session_dialog_new (GListModel  *sessions,
                                                    const gchar *selected_id,
                                                    GtkWindow   *parent);

G_END_DECLS
