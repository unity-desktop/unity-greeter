/* unity-greeter-session-list.h
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

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

/**
 * UNITY_GREETER_TYPE_SESSION:
 *
 * The #GType for #UnityGreeterSession.
 */
#define UNITY_GREETER_TYPE_SESSION (unity_greeter_session_get_type ())

/**
 * UnityGreeterSession:
 *
 * One desktop session entry discovered from a session `.desktop` file.
 */
G_DECLARE_FINAL_TYPE (UnityGreeterSession, unity_greeter_session,
                      UNITY_GREETER, SESSION, GObject)

/**
 * unity_greeter_session_get_id:
 * @self: a #UnityGreeterSession.
 *
 * Gets the stable identifier for the session.
 *
 * Returns: (transfer none): the session id.
 */
const gchar *unity_greeter_session_get_id            (UnityGreeterSession *self);

/**
 * unity_greeter_session_get_name:
 * @self: a #UnityGreeterSession.
 *
 * Gets the display name for the session.
 *
 * Returns: (transfer none): the display name.
 */
const gchar *unity_greeter_session_get_name          (UnityGreeterSession *self);

/**
 * unity_greeter_session_get_comment:
 * @self: a #UnityGreeterSession.
 *
 * Gets the description text for the session.
 *
 * Returns: (nullable) (transfer none): the description.
 */
const gchar *unity_greeter_session_get_comment       (UnityGreeterSession *self);

/**
 * unity_greeter_session_get_desktop_names:
 * @self: a #UnityGreeterSession.
 *
 * Gets the `XDG_CURRENT_DESKTOP` value to use for this session.
 *
 * Returns: (nullable) (transfer none): desktop names joined by `:`.
 */
const gchar *unity_greeter_session_get_desktop_names (UnityGreeterSession *self);

/**
 * unity_greeter_session_parse_command:
 * @self: a #UnityGreeterSession.
 * @error: return location for an error, or %NULL.
 *
 * Parses the session command line into an argument vector.
 *
 * Returns: (nullable) (transfer full): a %NULL-terminated argument vector.
 */
GStrv unity_greeter_session_parse_command (UnityGreeterSession *self,
                                           GError             **error);

/**
 * unity_greeter_session_build_environment:
 * @self: a #UnityGreeterSession.
 *
 * Builds environment variables for launching the session.
 *
 * Returns: (transfer full): a %NULL-terminated environment vector.
 */
GStrv unity_greeter_session_build_environment (UnityGreeterSession *self);

/**
 * unity_greeter_session_list_new:
 *
 * Scans system session directories and returns discovered sessions.
 *
 * Returns: (transfer full): a #GListModel of #UnityGreeterSession.
 */
GListModel *unity_greeter_session_list_new           (void);

/**
 * unity_greeter_session_list_new_from_dir:
 * @dir: path to a directory containing session `.desktop` files.
 *
 * Scans @dir and returns discovered sessions.
 *
 * Returns: (transfer full): a #GListModel of #UnityGreeterSession.
 */
GListModel *unity_greeter_session_list_new_from_dir  (const gchar *dir);

/**
 * unity_greeter_session_list_find:
 * @model: a #GListModel of #UnityGreeterSession objects.
 * @id: session id to search for.
 *
 * Finds a session by id.
 *
 * Returns: (nullable) (transfer full): the matching session.
 */
UnityGreeterSession *unity_greeter_session_list_find (GListModel  *model,
                                                      const gchar *id);

G_END_DECLS
