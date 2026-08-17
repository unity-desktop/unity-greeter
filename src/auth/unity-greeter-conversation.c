/* unity-greeter-conversation.c
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

#include "unity-greeter-conversation.h"

#include <string.h>

#include <astal-greet.h>

G_DEFINE_QUARK (unity-greeter-error-quark, unity_greeter_error)

struct _UnityGreeterConversation
{
  GObject parent_instance;

  AstalGreetGreeter *greeter;
  gchar             *username;
};

enum {
  SIGNAL_PROMPT,
  SIGNAL_MESSAGE,
  SIGNAL_AUTHENTICATED,
  SIGNAL_FAILED,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

G_DEFINE_FINAL_TYPE (UnityGreeterConversation, unity_greeter_conversation, G_TYPE_OBJECT)

static GError *
make_error (gboolean is_auth_error, const gchar *description)
{
  UnityGreeterErrorCode code = is_auth_error
                                 ? UNITY_GREETER_ERROR_AUTH
                                 : UNITY_GREETER_ERROR_OTHER;
  return g_error_new_literal (UNITY_GREETER_ERROR, code,
                              description != NULL ? description : "");
}

static void
drop_greeter (UnityGreeterConversation *self)
{
  if (self->greeter == NULL)
    return;
  g_signal_handlers_disconnect_by_data (self->greeter, self);
  g_clear_object (&self->greeter);
}

static gchar *
normalise_prompt_label (const gchar *label)
{
  gchar *copy = g_strdup (label != NULL ? label : "");
  g_strstrip (copy);
  gsize n = strlen (copy);
  while (n > 0 && copy[n - 1] == ':')
    copy[--n] = '\0';
  g_strstrip (copy);
  return copy;
}

static void
on_prompt_request (AstalGreetGreeter        *greeter,
                   const gchar              *label,
                   gboolean                  secret,
                   UnityGreeterConversation *self)
{
  (void) greeter;
  g_autofree gchar *clean = normalise_prompt_label (label);
  g_signal_emit (self, signals[SIGNAL_PROMPT], 0, clean, secret);
}

static void
on_visible_request (AstalGreetGreeter *g, const gchar *label, gpointer d)
{
  on_prompt_request (g, label, FALSE, d);
}

static void
on_secret_request (AstalGreetGreeter *g, const gchar *label, gpointer d)
{
  on_prompt_request (g, label, TRUE, d);
}

static void
on_info_message (AstalGreetGreeter *g, const gchar *text, gpointer d)
{
  (void) g;
  UnityGreeterConversation *self = d;
  g_signal_emit (self, signals[SIGNAL_MESSAGE], 0, text != NULL ? text : "", FALSE);
}

static void
on_error_message (AstalGreetGreeter *g, const gchar *text, gpointer d)
{
  (void) g;
  UnityGreeterConversation *self = d;
  g_signal_emit (self, signals[SIGNAL_MESSAGE], 0, text != NULL ? text : "", TRUE);
}

static void
on_authenticated (AstalGreetGreeter *g, gpointer d)
{
  (void) g;
  UnityGreeterConversation *self = d;
  g_signal_emit (self, signals[SIGNAL_AUTHENTICATED], 0);
}

static void
on_cancelled (AstalGreetGreeter *g, AstalGreetError *e, gpointer d)
{
  (void) g;
  UnityGreeterConversation *self = d;

  gboolean is_auth =
    astal_greet_error_get_error_type (e) == ASTAL_GREET_ERROR_TYPE_AUTH_ERROR;
  const gchar *desc = astal_greet_error_get_description (e);

  g_autoptr (GError) err = make_error (is_auth, desc);

  drop_greeter (self);
  g_signal_emit (self, signals[SIGNAL_FAILED], 0, err);
}

void
unity_greeter_conversation_begin (UnityGreeterConversation *self,
                                  const gchar              *username)
{
  g_return_if_fail (UNITY_GREETER_IS_CONVERSATION (self));
  g_return_if_fail (username != NULL);

  drop_greeter (self);
  self->greeter = astal_greet_greeter_new ();
  g_signal_connect_object (self->greeter, "visible-request",
                           G_CALLBACK (on_visible_request), self, G_CONNECT_DEFAULT);
  g_signal_connect_object (self->greeter, "secret-request",
                           G_CALLBACK (on_secret_request), self, G_CONNECT_DEFAULT);
  g_signal_connect_object (self->greeter, "info-message",
                           G_CALLBACK (on_info_message), self, G_CONNECT_DEFAULT);
  g_signal_connect_object (self->greeter, "error-message",
                           G_CALLBACK (on_error_message), self, G_CONNECT_DEFAULT);
  g_signal_connect_object (self->greeter, "authenticated",
                           G_CALLBACK (on_authenticated), self, G_CONNECT_DEFAULT);
  g_signal_connect_object (self->greeter, "cancelled",
                           G_CALLBACK (on_cancelled), self, G_CONNECT_DEFAULT);

  g_set_str (&self->username, username);
  astal_greet_greeter_create_session (self->greeter, self->username);
}

void
unity_greeter_conversation_answer (UnityGreeterConversation *self,
                                   const gchar              *response)
{
  g_return_if_fail (UNITY_GREETER_IS_CONVERSATION (self));
  g_return_if_fail (response != NULL);

  if (self->greeter != NULL)
    astal_greet_greeter_post_auth (self->greeter, response);
}

void
unity_greeter_conversation_start (UnityGreeterConversation  *self,
                                  gchar                    **cmd,
                                  gchar                    **env)
{
  g_return_if_fail (UNITY_GREETER_IS_CONVERSATION (self));
  g_return_if_fail (cmd != NULL);

  if (self->greeter == NULL)
    return;

  static gchar *empty[] = { NULL };
  gchar **env_effective = env != NULL ? env : empty;

  astal_greet_greeter_start_session (self->greeter,
                                     cmd, g_strv_length (cmd),
                                     env_effective, g_strv_length (env_effective),
                                     NULL, NULL);
}

void
unity_greeter_conversation_cancel (UnityGreeterConversation *self)
{
  g_return_if_fail (UNITY_GREETER_IS_CONVERSATION (self));
  drop_greeter (self);
}

static void
unity_greeter_conversation_dispose (GObject *object)
{
  UnityGreeterConversation *self = UNITY_GREETER_CONVERSATION (object);
  drop_greeter (self);
  G_OBJECT_CLASS (unity_greeter_conversation_parent_class)->dispose (object);
}

static void
unity_greeter_conversation_finalize (GObject *object)
{
  UnityGreeterConversation *self = UNITY_GREETER_CONVERSATION (object);
  g_clear_pointer (&self->username, g_free);
  G_OBJECT_CLASS (unity_greeter_conversation_parent_class)->finalize (object);
}

static void
unity_greeter_conversation_class_init (UnityGreeterConversationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  object_class->dispose  = unity_greeter_conversation_dispose;
  object_class->finalize = unity_greeter_conversation_finalize;

  signals[SIGNAL_PROMPT] = g_signal_new (
    "prompt", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
    0, NULL, NULL, NULL, G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_BOOLEAN);

  signals[SIGNAL_MESSAGE] = g_signal_new (
    "message", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
    0, NULL, NULL, NULL, G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_BOOLEAN);

  signals[SIGNAL_AUTHENTICATED] = g_signal_new (
    "authenticated", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
    0, NULL, NULL, NULL, G_TYPE_NONE, 0);

  signals[SIGNAL_FAILED] = g_signal_new (
    "failed", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
    0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_ERROR);
}

static void
unity_greeter_conversation_init (UnityGreeterConversation *self)
{
  (void) self;
}

UnityGreeterConversation *
unity_greeter_conversation_new (void)
{
  return g_object_new (UNITY_GREETER_TYPE_CONVERSATION, NULL);
}
