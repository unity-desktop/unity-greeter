/* unity-greeter-conversation.h
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

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * UnityGreeterErrorCode:
 * @UNITY_GREETER_ERROR_AUTH: authentication failure.
 * @UNITY_GREETER_ERROR_OTHER: non-authentication error.
 *
 * Error codes for #UNITY_GREETER_ERROR.
 */
typedef enum
{
  UNITY_GREETER_ERROR_AUTH,
  UNITY_GREETER_ERROR_OTHER,
} UnityGreeterErrorCode;

/**
 * UNITY_GREETER_ERROR:
 *
 * Error domain for Unity Greeter conversation operations.
 */
#define UNITY_GREETER_ERROR (unity_greeter_error_quark ())

/**
 * unity_greeter_error_quark:
 *
 * Gets the #GQuark for #UNITY_GREETER_ERROR.
 *
 * Returns: the error domain quark.
 */
GQuark unity_greeter_error_quark (void);

/**
 * UNITY_GREETER_TYPE_CONVERSATION:
 *
 * The #GType for #UnityGreeterConversation.
 */
#define UNITY_GREETER_TYPE_CONVERSATION (unity_greeter_conversation_get_type ())

/**
 * UnityGreeterConversation:
 *
 * Manages an authentication conversation with the greeter backend.
 */
G_DECLARE_FINAL_TYPE (UnityGreeterConversation, unity_greeter_conversation,
                      UNITY_GREETER, CONVERSATION, GObject)

/**
 * unity_greeter_conversation_new:
 *
 * Creates a new authentication conversation object.
 *
 * Returns: (transfer full): a new #UnityGreeterConversation.
 */
UnityGreeterConversation *unity_greeter_conversation_new (void);

/**
 * unity_greeter_conversation_begin:
 * @self: a #UnityGreeterConversation.
 * @username: user name to authenticate.
 *
 * Starts a new authentication conversation for @username.
 */
void unity_greeter_conversation_begin (UnityGreeterConversation *self,
                                       const gchar              *username);

/**
 * unity_greeter_conversation_answer:
 * @self: a #UnityGreeterConversation.
 * @response: response text for the active prompt.
 *
 * Sends a response to the active authentication prompt.
 */
void unity_greeter_conversation_answer (UnityGreeterConversation *self,
                                        const gchar              *response);

/**
 * unity_greeter_conversation_start:
 * @self: a #UnityGreeterConversation.
 * @cmd: %NULL-terminated command argument vector.
 * @env: (nullable): %NULL-terminated environment vector.
 *
 * Starts the user session for a completed authentication flow.
 */
void unity_greeter_conversation_start  (UnityGreeterConversation *self,
                                        const gchar * const      *cmd,
                                        const gchar * const      *env);

/**
 * unity_greeter_conversation_cancel:
 * @self: a #UnityGreeterConversation.
 *
 * Cancels the current authentication conversation.
 */
void unity_greeter_conversation_cancel (UnityGreeterConversation *self);

G_END_DECLS
