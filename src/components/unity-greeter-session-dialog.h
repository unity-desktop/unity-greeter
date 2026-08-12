#pragma once

#include <adwaita.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/**
 * UNITY_GREETER_TYPE_SESSION_DIALOG:
 *
 * The #GType for #UnityGreeterSessionDialog.
 */
#define UNITY_GREETER_TYPE_SESSION_DIALOG (unity_greeter_session_dialog_get_type ())

/**
 * UnityGreeterSessionDialog:
 *
 * Dialog used to choose a session before signing in.
 */
G_DECLARE_FINAL_TYPE (UnityGreeterSessionDialog, unity_greeter_session_dialog,
                      UNITY_GREETER, SESSION_DIALOG, AdwDialog)

/**
 * unity_greeter_session_dialog_new:
 * @sessions: list model containing #UnityGreeterSession objects.
 * @selected_id: (nullable): id of the session to preselect.
 *
 * Creates a new session picker dialog.
 *
 * Returns: (transfer full): a new #AdwDialog.
 */
AdwDialog *unity_greeter_session_dialog_new (GListModel  *sessions,
                                             const gchar *selected_id);

G_END_DECLS
