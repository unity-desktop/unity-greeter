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

G_DECLARE_FINAL_TYPE (UnityGreeterConversation, unity_greeter_conversation,
                      UNITY_GREETER, CONVERSATION, GObject)

UnityGreeterConversation *unity_greeter_conversation_new (void);

void unity_greeter_conversation_begin (UnityGreeterConversation *self,
                                       const gchar              *username);

void unity_greeter_conversation_answer (UnityGreeterConversation *self,
                                        const gchar              *response);

void unity_greeter_conversation_start  (UnityGreeterConversation  *self,
                                        const gchar * const       *cmd,
                                        const gchar * const       *env);

void unity_greeter_conversation_cancel (UnityGreeterConversation *self);

G_END_DECLS
