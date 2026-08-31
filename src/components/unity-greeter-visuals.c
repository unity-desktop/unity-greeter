/* unity-greeter-visuals.c
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

#include "unity-greeter-visuals.h"

/* Avatar initials, display name and the account picture. */
void
unity_greeter_apply_identity (AdwAvatar        *avatar,
                              GtkLabel         *name_label,
                              ActUser *user)
{
  const gchar *display = unity_greeter_user_display_name (user);
  adw_avatar_set_text (avatar, display);
  gtk_label_set_text  (name_label, display != NULL ? display : "");

  const gchar *icon = act_user_get_icon_file (user);
  g_autoptr (GdkTexture) tex =
    (icon != NULL && *icon != '\0') ? gdk_texture_new_from_filename (icon, NULL)
                                    : NULL;
  adw_avatar_set_custom_image (avatar,
                               tex != NULL ? GDK_PAINTABLE (tex) : NULL);
}

/* @basename is the file unity-shell mirrors under the user directory. */
void
unity_greeter_apply_wallpaper (GtkPicture       *picture,
                               ActUser *user,
                               const gchar      *basename)
{
  const gchar *user_name = act_user_get_user_name (user);
  if (user_name == NULL || *user_name == '\0')
    return;

  g_autofree gchar *path =
    g_build_filename (UNITY_GREETER_MIRROR_ROOT, user_name, basename, NULL);
  g_autoptr (GdkTexture) tex = gdk_texture_new_from_filename (path, NULL);

  gtk_picture_set_paintable (picture,
                             tex != NULL ? GDK_PAINTABLE (tex) : NULL);
}

void
unity_greeter_clear_status (GtkLabel *label)
{
  gtk_widget_set_visible (GTK_WIDGET (label), FALSE);
  gtk_widget_remove_css_class (GTK_WIDGET (label), "error");
  gtk_widget_remove_css_class (GTK_WIDGET (label), "dim-label");
}

/* One status line: hidden when empty, red when it reports a failure. */
void
unity_greeter_set_status_text (GtkLabel    *label,
                               const gchar *text,
                               gboolean     is_error)
{
  unity_greeter_clear_status (label);

  if (text == NULL || *text == '\0')
    return;

  gtk_label_set_text (label, text);
  gtk_widget_add_css_class (GTK_WIDGET (label), is_error ? "error" : "dim-label");
  gtk_widget_set_visible   (GTK_WIDGET (label), TRUE);
}
