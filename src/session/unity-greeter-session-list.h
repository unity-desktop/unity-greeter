/* unity-greeter-session-list.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_SESSION (unity_greeter_session_get_type ())

/* One desktop session discovered from a session `.desktop` file. */
G_DECLARE_FINAL_TYPE (UnityGreeterSession, unity_greeter_session,
                      UNITY_GREETER, SESSION, GObject)

const gchar *unity_greeter_session_get_id            (UnityGreeterSession *self);
const gchar *unity_greeter_session_get_name          (UnityGreeterSession *self);
const gchar *unity_greeter_session_get_comment       (UnityGreeterSession *self);
/* XDG_CURRENT_DESKTOP value, names joined by `:`. */
const gchar *unity_greeter_session_get_desktop_names (UnityGreeterSession *self);

/* Session command line split into an argument vector. */
GStrv unity_greeter_session_parse_command (UnityGreeterSession *self,
                                           GError             **error);

/* Environment for launching the session. */
GStrv unity_greeter_session_build_environment (UnityGreeterSession *self);

/* Sessions found in the system wayland-sessions directories. */
GListModel *unity_greeter_session_list_new (void);

/* Borrowed match, or NULL. The model keeps the reference. */
UnityGreeterSession *unity_greeter_session_list_find (GListModel  *model,
                                                      const gchar *id);

G_END_DECLS
