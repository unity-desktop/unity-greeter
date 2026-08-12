#include "unity-greeter-conversation.h"

#include <astal-greet.h>

G_DEFINE_QUARK (unity-greeter-error-quark, unity_greeter_error)

static GError *
make_error (gboolean is_auth_error, const gchar *description)
{
  UnityGreeterErrorCode code = is_auth_error
                                 ? UNITY_GREETER_ERROR_AUTH
                                 : UNITY_GREETER_ERROR_OTHER;
  return g_error_new_literal (UNITY_GREETER_ERROR, code,
                              description != NULL ? description : "");
}

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

static void
drop_greeter (UnityGreeterConversation *self)
{
  if (self->greeter == NULL)
    return;
  g_signal_handlers_disconnect_by_data (self->greeter, self);
  g_clear_object (&self->greeter);
}

static void
on_visible_request (AstalGreetGreeter *g, const gchar *label, gpointer d)
{
  (void) g;
  g_signal_emit (d, signals[SIGNAL_PROMPT], 0, label != NULL ? label : "", FALSE);
}

static void
on_secret_request (AstalGreetGreeter *g, const gchar *label, gpointer d)
{
  (void) g;
  g_signal_emit (d, signals[SIGNAL_PROMPT], 0, label != NULL ? label : "", TRUE);
}

static void
on_info_message (AstalGreetGreeter *g, const gchar *text, gpointer d)
{
  (void) g;
  g_signal_emit (d, signals[SIGNAL_MESSAGE], 0, text != NULL ? text : "", FALSE);
}

static void
on_error_message (AstalGreetGreeter *g, const gchar *text, gpointer d)
{
  (void) g;
  g_signal_emit (d, signals[SIGNAL_MESSAGE], 0, text != NULL ? text : "", TRUE);
}

static void
on_authenticated (AstalGreetGreeter *g, gpointer d)
{
  (void) g;
  g_signal_emit (d, signals[SIGNAL_AUTHENTICATED], 0);
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
                           G_CALLBACK (on_visible_request), self, 0);
  g_signal_connect_object (self->greeter, "secret-request",
                           G_CALLBACK (on_secret_request), self, 0);
  g_signal_connect_object (self->greeter, "info-message",
                           G_CALLBACK (on_info_message), self, 0);
  g_signal_connect_object (self->greeter, "error-message",
                           G_CALLBACK (on_error_message), self, 0);
  g_signal_connect_object (self->greeter, "authenticated",
                           G_CALLBACK (on_authenticated), self, 0);
  g_signal_connect_object (self->greeter, "cancelled",
                           G_CALLBACK (on_cancelled), self, 0);

  g_free (self->username);
  self->username = g_strdup (username);

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
unity_greeter_conversation_start (UnityGreeterConversation *self,
                                  const gchar * const      *cmd,
                                  const gchar * const      *env)
{
  g_return_if_fail (UNITY_GREETER_IS_CONVERSATION (self));
  g_return_if_fail (cmd != NULL);

  if (self->greeter == NULL)
    return;

  static gchar *empty[] = { NULL };
  const gchar * const *env_effective =
    env != NULL ? env : (const gchar * const *) empty;

  gint cmd_len = 0;
  for (const gchar * const *p = cmd; *p != NULL; p++)
    cmd_len++;
  gint env_len = 0;
  for (const gchar * const *p = env_effective; *p != NULL; p++)
    env_len++;

  astal_greet_greeter_start_session (self->greeter,
                                     (gchar **) cmd, cmd_len,
                                     (gchar **) env_effective, env_len,
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
