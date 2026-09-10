/* unity-greeter-conversation.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

typedef enum
{
  UNITY_GREETER_ERROR_AUTH,
  UNITY_GREETER_ERROR_OTHER,
} UnityGreeterErrorCode;

#define UNITY_GREETER_ERROR (unity_greeter_error_quark ())

GQuark unity_greeter_error_quark (void);

#define UNITY_GREETER_TYPE_CONVERSATION (unity_greeter_conversation_get_type ())

/* Drives one login attempt against greetd. Relays PAM prompts, info and
   error messages to the UI unchanged, and emits a terminal signal when
   the flow ends: "prompt", "message", "authenticated", "failed". */
G_DECLARE_FINAL_TYPE (UnityGreeterConversation, unity_greeter_conversation,
                      UNITY_GREETER, CONVERSATION, GObject)

UnityGreeterConversation *unity_greeter_conversation_new (void);

/* Opens a fresh greetd session for @username. */
void unity_greeter_conversation_begin (UnityGreeterConversation *self,
                                       const gchar              *username);

/* Answers the active prompt. */
void unity_greeter_conversation_answer (UnityGreeterConversation *self,
                                        const gchar              *response);

/* Runs @cmd once PAM has already reported success. */
void unity_greeter_conversation_start  (UnityGreeterConversation  *self,
                                        gchar                    **cmd,
                                        gchar                    **env);

/* Drops the running session. */
void unity_greeter_conversation_cancel (UnityGreeterConversation *self);

G_END_DECLS
