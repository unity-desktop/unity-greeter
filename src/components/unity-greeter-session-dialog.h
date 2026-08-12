#pragma once

#include <adwaita.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_SESSION_DIALOG (unity_greeter_session_dialog_get_type ())

G_DECLARE_FINAL_TYPE (UnityGreeterSessionDialog, unity_greeter_session_dialog,
                      UNITY_GREETER, SESSION_DIALOG, AdwDialog)

AdwDialog *unity_greeter_session_dialog_new (GListModel *sessions,
                                             const gchar *selected_id);

G_END_DECLS
