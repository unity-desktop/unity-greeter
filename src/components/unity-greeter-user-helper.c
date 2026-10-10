/* unity-greeter-user-helper.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "unity-greeter-user-helper.h"

#include "unity-greeter-publish.h"

void
unity_greeter_apply_identity (AdwAvatar *avatar,
                              GtkLabel  *name_label,
                              ActUser   *user)
{
  const gchar *display = unity_greeter_user_display_name (user);
  adw_avatar_set_text (avatar, display);
  gtk_label_set_text (name_label, display != NULL ? display : "");

  const gchar *icon = act_user_get_icon_file (user);
  g_autoptr (GdkTexture) texture =
    (icon != NULL && *icon != '\0') ? gdk_texture_new_from_filename (icon, NULL)
                                    : NULL;

  adw_avatar_set_custom_image (avatar,
                               texture != NULL ? GDK_PAINTABLE (texture) : NULL);
}

void
unity_greeter_apply_wallpaper (GtkPicture *picture,
                               ActUser    *user)
{
  g_autofree gchar *path = unity_greeter_publish_path (act_user_get_user_name (user),
                                                       UNITY_GREETER_PUBLISH_WALLPAPER);
  gtk_picture_set_filename (picture, path);
}

void
unity_greeter_set_status_text (GtkLabel    *label,
                               const gchar *text,
                               gboolean     is_error)
{
  gtk_widget_set_visible (GTK_WIDGET (label), FALSE);
  gtk_widget_remove_css_class (GTK_WIDGET (label), "error");
  gtk_widget_remove_css_class (GTK_WIDGET (label), "dim-label");

  if (text == NULL || *text == '\0')
    return;

  gtk_label_set_text (label, text);
  gtk_widget_add_css_class (GTK_WIDGET (label), is_error ? "error" : "dim-label");
  gtk_widget_set_visible (GTK_WIDGET (label), TRUE);
}
