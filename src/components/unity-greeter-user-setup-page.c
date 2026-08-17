/* unity-greeter-user-setup-page.c
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

#include "unity-greeter-user-setup-page.h"

#include <glib/gi18n.h>

#include "unity-greeter-defs.h"
#include "unity-greeter-strength.h"
#include "unity-greeter-user-page.h"

#define STRENGTH_ACCEPT_LEVEL 2

struct _UnityGreeterUserSetupPage
{
  AdwNavigationPage parent_instance;

  GtkPicture          *wallpaper;
  AdwAvatar           *avatar;
  GtkLabel            *name_label;
  AdwPasswordEntryRow *entry;
  AdwPasswordEntryRow *confirm_entry;
  GtkLevelBar         *strength_bar;
  GtkLabel            *hint_label;
  AdwButtonRow        *submit_button;
  GtkButton           *back;

  UnityGreeterUser     *user;
  GListModel           *sessions;
  UnityGreeterStrength *strength;
};

G_DEFINE_FINAL_TYPE (UnityGreeterUserSetupPage, unity_greeter_user_setup_page,
                     ADW_TYPE_NAVIGATION_PAGE)

static void
apply_identity (UnityGreeterUserSetupPage *self)
{
  const gchar *display = unity_greeter_user_get_display_name (self->user);
  adw_avatar_set_text (self->avatar, display);
  gtk_label_set_text  (self->name_label, display != NULL ? display : "");

  const gchar *icon = unity_greeter_user_get_icon_file (self->user);
  if (icon == NULL || *icon == '\0')
    return;

  g_autoptr (GdkTexture) tex = gdk_texture_new_from_filename (icon, NULL);
  if (tex != NULL)
    adw_avatar_set_custom_image (self->avatar, GDK_PAINTABLE (tex));
}

static void
apply_wallpaper (UnityGreeterUserSetupPage *self)
{
  const gchar *user_name = unity_greeter_user_get_user_name (self->user);
  if (user_name == NULL || *user_name == '\0')
    return;

  g_autofree gchar *bg =
    g_build_filename (UNITY_GREETER_MIRROR_ROOT, user_name, "background.png", NULL);
  g_autoptr (GdkTexture) tex = gdk_texture_new_from_filename (bg, NULL);
  if (tex != NULL)
    gtk_picture_set_paintable (self->wallpaper, GDK_PAINTABLE (tex));
}

static void
show_hint (UnityGreeterUserSetupPage *self,
           const gchar               *text,
           gboolean                   is_error)
{
  gtk_label_set_text (self->hint_label, text != NULL ? text : "");
  gtk_widget_set_visible (GTK_WIDGET (self->hint_label),
                          text != NULL && *text != '\0');
  gtk_widget_remove_css_class (GTK_WIDGET (self->hint_label), "error");
  gtk_widget_remove_css_class (GTK_WIDGET (self->hint_label), "dim-label");
  gtk_widget_add_css_class    (GTK_WIDGET (self->hint_label),
                               is_error ? "error" : "dim-label");
}

static void
validate (UnityGreeterUserSetupPage *self)
{
  const gchar *password = gtk_editable_get_text (GTK_EDITABLE (self->entry));
  const gchar *confirm  = gtk_editable_get_text (GTK_EDITABLE (self->confirm_entry));
  const gchar *username = unity_greeter_user_get_user_name (self->user);

  const gchar *hint = NULL;
  const gchar *level_hint = NULL;
  gint         level = 0;
  unity_greeter_strength_check (self->strength, password, username,
                                &hint, &level, &level_hint);

  gtk_level_bar_set_value (self->strength_bar, level);

  gboolean strong_enough = level >= STRENGTH_ACCEPT_LEVEL;
  gboolean match         = (password != NULL) && (confirm != NULL) &&
                           g_strcmp0 (password, confirm) == 0;
  gboolean confirm_typed = confirm != NULL && *confirm != '\0';

  if (confirm_typed && !match)
    show_hint (self, _("The passwords do not match."), TRUE);
  else if (password != NULL && *password != '\0' && !strong_enough && hint != NULL)
    show_hint (self, hint, TRUE);
  else if (password != NULL && *password != '\0' && level_hint != NULL)
    show_hint (self, level_hint, FALSE);
  else
    show_hint (self, NULL, FALSE);

  gtk_widget_set_sensitive (GTK_WIDGET (self->submit_button),
                            strong_enough && match);
}

static void
on_entry_changed (GtkEditable *editable, UnityGreeterUserSetupPage *self)
{
  (void) editable;
  validate (self);
}

static void
on_submit (GtkWidget *source, UnityGreeterUserSetupPage *self)
{
  (void) source;

  if (!gtk_widget_get_sensitive (GTK_WIDGET (self->submit_button)))
    return;

  const gchar *password = gtk_editable_get_text (GTK_EDITABLE (self->entry));
  unity_greeter_user_set_password (self->user, password);

  /* We do not wait to see whether the write took. If something rejected
     it, the user will land back on this page next time. Otherwise we
     move them on to the login page for the same account. */
  AdwNavigationView *view =
    ADW_NAVIGATION_VIEW (gtk_widget_get_ancestor (GTK_WIDGET (self),
                                                  ADW_TYPE_NAVIGATION_VIEW));
  if (view == NULL)
    return;

  AdwNavigationPage *login = unity_greeter_user_page_new (self->user, self->sessions);
  adw_navigation_view_pop (view);
  adw_navigation_view_push (view, login);
}

static void
on_page_shown (AdwNavigationPage *page, gpointer user_data)
{
  (void) user_data;
  UnityGreeterUserSetupPage *self = UNITY_GREETER_USER_SETUP_PAGE (page);
  gtk_widget_grab_focus (GTK_WIDGET (self->entry));
}

static void
unity_greeter_user_setup_page_dispose (GObject *object)
{
  UnityGreeterUserSetupPage *self = UNITY_GREETER_USER_SETUP_PAGE (object);
  gtk_widget_dispose_template (GTK_WIDGET (self), UNITY_GREETER_TYPE_USER_SETUP_PAGE);
  g_clear_object (&self->strength);
  g_clear_object (&self->sessions);
  g_clear_object (&self->user);
  G_OBJECT_CLASS (unity_greeter_user_setup_page_parent_class)->dispose (object);
}

static void
unity_greeter_user_setup_page_class_init (UnityGreeterUserSetupPageClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = unity_greeter_user_setup_page_dispose;

  gtk_widget_class_set_template_from_resource (widget_class,
    "/org/unity/Greeter/unity-greeter-user-setup-page.ui");

  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserSetupPage, wallpaper);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserSetupPage, avatar);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserSetupPage, name_label);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserSetupPage, entry);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserSetupPage, confirm_entry);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserSetupPage, strength_bar);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserSetupPage, hint_label);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserSetupPage, submit_button);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserSetupPage, back);

  gtk_widget_class_bind_template_callback (widget_class, on_page_shown);
  gtk_widget_class_bind_template_callback (widget_class, on_entry_changed);
  gtk_widget_class_bind_template_callback (widget_class, on_submit);
}

static void
unity_greeter_user_setup_page_init (UnityGreeterUserSetupPage *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

AdwNavigationPage *
unity_greeter_user_setup_page_new (UnityGreeterUser *user, GListModel *sessions)
{
  g_return_val_if_fail (UNITY_GREETER_IS_USER (user), NULL);
  g_return_val_if_fail (G_IS_LIST_MODEL (sessions), NULL);

  UnityGreeterUserSetupPage *self =
    g_object_new (UNITY_GREETER_TYPE_USER_SETUP_PAGE, NULL);
  self->user     = g_object_ref (user);
  self->sessions = g_object_ref (sessions);
  self->strength = unity_greeter_strength_new ();

  apply_identity (self);
  apply_wallpaper (self);
  validate (self);

  return ADW_NAVIGATION_PAGE (self);
}
