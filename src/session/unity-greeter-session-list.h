#pragma once

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_SESSION (unity_greeter_session_get_type ())

G_DECLARE_FINAL_TYPE (UnityGreeterSession, unity_greeter_session,
                      UNITY_GREETER, SESSION, GObject)

const gchar *unity_greeter_session_get_id             (UnityGreeterSession *self);
const gchar *unity_greeter_session_get_name           (UnityGreeterSession *self);
const gchar *unity_greeter_session_get_comment        (UnityGreeterSession *self);
const gchar *unity_greeter_session_get_desktop_names  (UnityGreeterSession *self);

GStrv unity_greeter_session_parse_command (UnityGreeterSession *self,
                                           GError             **error);

GStrv unity_greeter_session_build_environment (UnityGreeterSession *self);

GListModel *unity_greeter_session_list_new           (void);

GListModel *unity_greeter_session_list_new_from_dir  (const gchar *dir);

UnityGreeterSession *unity_greeter_session_list_find (GListModel *model,
                                                      const gchar *id);

G_END_DECLS
