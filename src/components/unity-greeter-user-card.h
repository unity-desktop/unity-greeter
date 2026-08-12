#pragma once

#include <adwaita.h>
#include <gtk/gtk.h>

#include "unity-greeter-user.h"

G_BEGIN_DECLS

/**
 * UNITY_GREETER_TYPE_USER_CARD:
 *
 * The #GType for #UnityGreeterUserCard.
 */
#define UNITY_GREETER_TYPE_USER_CARD (unity_greeter_user_card_get_type ())

/**
 * UnityGreeterUserCard:
 *
 * Widget showing a user entry on the greeter screen.
 */
G_DECLARE_FINAL_TYPE (UnityGreeterUserCard, unity_greeter_user_card,
                      UNITY_GREETER, USER_CARD, GtkBox)

/**
 * unity_greeter_user_card_new:
 * @user: the user to present.
 *
 * Creates a new user card widget.
 *
 * Returns: (transfer full): a new #GtkWidget.
 */
GtkWidget *unity_greeter_user_card_new (UnityGreeterUser *user);

/**
 * unity_greeter_user_card_get_user:
 * @self: a #UnityGreeterUserCard.
 *
 * Gets the user associated with the card.
 *
 * Returns: (transfer none): the associated #UnityGreeterUser.
 */
UnityGreeterUser *unity_greeter_user_card_get_user (UnityGreeterUserCard *self);

G_END_DECLS
