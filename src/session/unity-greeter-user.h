#pragma once

#include <act/act.h>
#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_USER (unity_greeter_user_get_type ())

G_DECLARE_FINAL_TYPE (UnityGreeterUser, unity_greeter_user,
                      UNITY_GREETER, USER, GObject)

UnityGreeterUser *unity_greeter_user_new (ActUser *user);

GListModel *unity_greeter_users_new (void);

void unity_greeter_user_persist_session (UnityGreeterUser *self,
                                         const gchar      *session_id);

const gchar *unity_greeter_user_get_user_name       (UnityGreeterUser *self);
const gchar *unity_greeter_user_get_display_name    (UnityGreeterUser *self);
const gchar *unity_greeter_user_get_icon_file       (UnityGreeterUser *self);
gint         unity_greeter_user_get_login_frequency (UnityGreeterUser *self);
gint         unity_greeter_user_get_password_mode   (UnityGreeterUser *self);
const gchar *unity_greeter_user_get_session         (UnityGreeterUser *self);

G_END_DECLS
