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
