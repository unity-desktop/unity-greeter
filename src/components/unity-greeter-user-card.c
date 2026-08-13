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

#include "unity-greeter-defs.h"

struct _UnityGreeterUserCard
{
  GtkBox parent_instance;

  GtkButton  *card;
  GtkOverlay *frame;
  GtkPicture *wallpaper;
  AdwAvatar  *avatar;
  GtkLabel   *name_label;

  UnityGreeterUser *user;
};

enum {
  SIGNAL_ACTIVATED,
  N_SIGNALS,
};

static guint signals[N_SIGNALS];

G_DEFINE_FINAL_TYPE (UnityGreeterUserCard, unity_greeter_user_card, GTK_TYPE_BOX)

static void
apply_avatar_image (UnityGreeterUserCard *self)
{
  const gchar *icon = unity_greeter_user_get_icon_file (self->user);
  if (icon == NULL || *icon == '\0')
    {
      adw_avatar_set_custom_image (self->avatar, NULL);
      return;
    }

  g_autoptr (GdkTexture) texture = gdk_texture_new_from_filename (icon, NULL);
  adw_avatar_set_custom_image (self->avatar,
                               texture != NULL ? GDK_PAINTABLE (texture) : NULL);
}

static void
apply_wallpaper (UnityGreeterUserCard *self)
{
  const gchar *name = unity_greeter_user_get_user_name (self->user);
  if (name == NULL || *name == '\0')
    return;

  g_autofree gchar *path =
    g_build_filename (UNITY_GREETER_MIRROR_ROOT, name, "card.png", NULL);
  g_autoptr (GdkTexture) texture = gdk_texture_new_from_filename (path, NULL);

  gtk_picture_set_paintable (self->wallpaper,
                             texture != NULL ? GDK_PAINTABLE (texture) : NULL);
}

static void
apply_name (UnityGreeterUserCard *self)
{
  const gchar *name = unity_greeter_user_get_display_name (self->user);
  gtk_label_set_text (self->name_label, name != NULL ? name : "");
  adw_avatar_set_text  (self->avatar,    name);
}

static void
on_card_clicked (GtkButton *button, gpointer user_data)
{
  (void) button;
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
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserCard, card);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserCard, frame);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserCard, wallpaper);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserCard, avatar);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserCard, name_label);

  gtk_widget_class_set_css_name (widget_class, "user-card");
}

static void
unity_greeter_user_card_init (UnityGreeterUserCard *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget *
unity_greeter_user_card_new (UnityGreeterUser *user)
{
  g_return_val_if_fail (UNITY_GREETER_IS_USER (user), NULL);

  UnityGreeterUserCard *self = g_object_new (UNITY_GREETER_TYPE_USER_CARD, NULL);
  self->user = g_object_ref (user);

  gtk_overlay_set_measure_overlay (self->frame, GTK_WIDGET (self->avatar), TRUE);

  g_signal_connect (self->card, "clicked", G_CALLBACK (on_card_clicked), self);

  apply_name (self);
  apply_avatar_image (self);
  apply_wallpaper (self);

  return GTK_WIDGET (self);
}

UnityGreeterUser *
unity_greeter_user_card_get_user (UnityGreeterUserCard *self)
{
  g_return_val_if_fail (UNITY_GREETER_IS_USER_CARD (self), NULL);
  return self->user;
}
