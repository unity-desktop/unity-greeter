#pragma once

#include <adwaita.h>
#include <gtk/gtk.h>

#include "unity-greeter-user.h"

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_USER_CARD (unity_greeter_user_card_get_type ())

G_DECLARE_FINAL_TYPE (UnityGreeterUserCard, unity_greeter_user_card,
                      UNITY_GREETER, USER_CARD, GtkBox)

GtkWidget *unity_greeter_user_card_new (UnityGreeterUser *user);

UnityGreeterUser *unity_greeter_user_card_get_user (UnityGreeterUserCard *self);

G_END_DECLS
