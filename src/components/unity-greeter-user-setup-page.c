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

#include "unity-greeter-strength.h"
#include "unity-greeter-user-page.h"
#include "unity-greeter-visuals.h"

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

  ActUser *user;
  GListModel       *sessions;
};

G_DEFINE_FINAL_TYPE (UnityGreeterUserSetupPage, unity_greeter_user_setup_page,
                     ADW_TYPE_NAVIGATION_PAGE)

static void
validate (UnityGreeterUserSetupPage *self)
{
  const gchar *password = gtk_editable_get_text (GTK_EDITABLE (self->entry));
  const gchar *confirm  = gtk_editable_get_text (GTK_EDITABLE (self->confirm_entry));
  const gchar *username = act_user_get_user_name (self->user);

  gboolean is_error = FALSE;
  gint     level    = 0;
  g_autofree gchar *message =
    unity_greeter_strength_check (password, username, &is_error, &level);

  gtk_level_bar_set_value (self->strength_bar, level);

  gboolean strong_enough = level >= STRENGTH_ACCEPT_LEVEL;
  gboolean match         = (password != NULL) && (confirm != NULL) &&
                           g_strcmp0 (password, confirm) == 0;
  gboolean confirm_typed = confirm != NULL && *confirm != '\0';

  /* Mismatch beats strength: if the two fields disagree the visitor
     needs to know that first, so it takes over the hint line. */
  if (confirm_typed && !match)
    unity_greeter_set_status_text (self->hint_label, _("The passwords do not match."), TRUE);
  else
    unity_greeter_set_status_text (self->hint_label, message, is_error);

  gtk_widget_set_sensitive (GTK_WIDGET (self->submit_button),
                            strong_enough && match);
}

static void
on_entry_changed (GtkEditable *editable, UnityGreeterUserSetupPage *self)
{
  validate (self);
}

static void
on_submit (GtkWidget *source, UnityGreeterUserSetupPage *self)
{
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
  UnityGreeterUserSetupPage *self = UNITY_GREETER_USER_SETUP_PAGE (page);
  gtk_widget_grab_focus (GTK_WIDGET (self->entry));
}

static void
on_user_changed (ActUser *user, gpointer data)
{
  UnityGreeterUserSetupPage *self = data;
  unity_greeter_apply_identity  (self->avatar, self->name_label, user);
  unity_greeter_apply_wallpaper (self->wallpaper, user, "background.png");
}

static void
unity_greeter_user_setup_page_dispose (GObject *object)
{
  UnityGreeterUserSetupPage *self = UNITY_GREETER_USER_SETUP_PAGE (object);
  gtk_widget_dispose_template (GTK_WIDGET (self), UNITY_GREETER_TYPE_USER_SETUP_PAGE);
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
unity_greeter_user_setup_page_new (ActUser *user, GListModel *sessions)
{
  g_return_val_if_fail (ACT_IS_USER (user), NULL);
  g_return_val_if_fail (G_IS_LIST_MODEL (sessions), NULL);

  UnityGreeterUserSetupPage *self =
    g_object_new (UNITY_GREETER_TYPE_USER_SETUP_PAGE, NULL);
  self->user     = g_object_ref (user);
  self->sessions = g_object_ref (sessions);

  unity_greeter_apply_identity (self->avatar, self->name_label, self->user);
  unity_greeter_apply_wallpaper (self->wallpaper, self->user, "background.png");
  g_signal_connect_object (self->user, "changed",
                           G_CALLBACK (on_user_changed), self, G_CONNECT_DEFAULT);
  validate (self);

  return ADW_NAVIGATION_PAGE (self);
}
