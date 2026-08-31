/* unity-greeter-user-card.c
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

#include "unity-greeter-user-card.h"

#include "unity-greeter-visuals.h"

struct _UnityGreeterUserCard
{
  GtkBox parent_instance;

  GtkOverlay *frame;
  GtkPicture *wallpaper;
  AdwAvatar  *avatar;
  GtkLabel   *name_label;

  ActUser *user;
};

enum {
  SIGNAL_ACTIVATED,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

G_DEFINE_FINAL_TYPE (UnityGreeterUserCard, unity_greeter_user_card, GTK_TYPE_BOX)

static void
on_card_clicked (GtkButton *button, gpointer user_data)
{
  g_signal_emit (UNITY_GREETER_USER_CARD (user_data), signals[SIGNAL_ACTIVATED], 0);
}

static void
unity_greeter_user_card_dispose (GObject *object)
{
  UnityGreeterUserCard *self = UNITY_GREETER_USER_CARD (object);

  gtk_widget_dispose_template (GTK_WIDGET (self), UNITY_GREETER_TYPE_USER_CARD);
  g_clear_object (&self->user);

  G_OBJECT_CLASS (unity_greeter_user_card_parent_class)->dispose (object);
}

static void
unity_greeter_user_card_class_init (UnityGreeterUserCardClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = unity_greeter_user_card_dispose;

  signals[SIGNAL_ACTIVATED] = g_signal_new (
    "activated", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
    0, NULL, NULL, NULL, G_TYPE_NONE, 0);

  gtk_widget_class_set_template_from_resource (
    widget_class, "/org/unity/Greeter/unity-greeter-user-card.ui");
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserCard, frame);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserCard, wallpaper);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserCard, avatar);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserCard, name_label);

  gtk_widget_class_bind_template_callback (widget_class, on_card_clicked);

  gtk_widget_class_set_css_name (widget_class, "user-card");
}

static void
unity_greeter_user_card_init (UnityGreeterUserCard *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget *
unity_greeter_user_card_new (ActUser *user)
{
  g_return_val_if_fail (ACT_IS_USER (user), NULL);

  UnityGreeterUserCard *self = g_object_new (UNITY_GREETER_TYPE_USER_CARD, NULL);
  self->user = g_object_ref (user);

  gtk_overlay_set_measure_overlay (self->frame, GTK_WIDGET (self->avatar), TRUE);

  unity_greeter_apply_identity (self->avatar, self->name_label, self->user);
  unity_greeter_apply_wallpaper (self->wallpaper, self->user, "card.png");

  return GTK_WIDGET (self);
}

ActUser *
unity_greeter_user_card_get_user (UnityGreeterUserCard *self)
{
  g_return_val_if_fail (UNITY_GREETER_IS_USER_CARD (self), NULL);
  return self->user;
}
