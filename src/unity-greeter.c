/* unity-greeter.c
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

#include "unity-greeter.h"

#include <glib/gi18n.h>

#include "unity-greeter-idle.h"
#include "unity-greeter-user-card.h"
#include "unity-greeter-user-page.h"
#include "unity-greeter-user-setup-page.h"

struct _UnityGreeter
{
  AdwApplicationWindow parent_instance;

  AdwNavigationView *nav;
  AdwWrapBox        *cards;

  GListModel *users;
  GListModel *sessions;
};

G_DEFINE_FINAL_TYPE (UnityGreeter, unity_greeter, ADW_TYPE_APPLICATION_WINDOW)

static void
on_card_activated (UnityGreeterUserCard *card, gpointer user_data)
{
  UnityGreeter     *self = UNITY_GREETER (user_data);
  UnityGreeterUser *user = unity_greeter_user_card_get_user (card);

  AdwNavigationPage *visible = adw_navigation_view_get_visible_page (self->nav);
  if (UNITY_GREETER_IS_USER_PAGE (visible) ||
      UNITY_GREETER_IS_USER_SETUP_PAGE (visible))
    return;

  AdwNavigationPage *page =
    unity_greeter_user_get_password_mode (user) == ACT_USER_PASSWORD_MODE_SET_AT_LOGIN
      ? unity_greeter_user_setup_page_new (user, self->sessions)
      : unity_greeter_user_page_new (user, self->sessions);
  adw_navigation_view_push (self->nav, page);
}

static void
sync_cards (UnityGreeter *self)
{
  guint n = g_list_model_get_n_items (self->users);

  adw_wrap_box_remove_all (self->cards);
  for (guint i = 0; i < n; i++)
    {
      g_autoptr (UnityGreeterUser) user =
        UNITY_GREETER_USER (g_list_model_get_item (self->users, i));
      GtkWidget *card = unity_greeter_user_card_new (user);
      g_signal_connect (card, "activated", G_CALLBACK (on_card_activated), self);
      adw_wrap_box_append (self->cards, card);
    }
}

static void
on_users_changed (GListModel *model,
                  guint       position,
                  guint       removed,
                  guint       added,
                  gpointer    user_data)
{
  (void) model; (void) position; (void) removed; (void) added;
  sync_cards (UNITY_GREETER (user_data));
}

static void
unity_greeter_dispose (GObject *object)
{
  UnityGreeter *self = UNITY_GREETER (object);

  gtk_widget_dispose_template (GTK_WIDGET (self), UNITY_TYPE_GREETER);
  g_clear_object (&self->users);
  g_clear_object (&self->sessions);

  G_OBJECT_CLASS (unity_greeter_parent_class)->dispose (object);
}

static void
unity_greeter_class_init (UnityGreeterClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = unity_greeter_dispose;

  g_type_ensure (UNITY_GREETER_TYPE_USER_CARD);
  g_type_ensure (UNITY_GREETER_TYPE_USER_PAGE);
  g_type_ensure (UNITY_GREETER_TYPE_USER_SETUP_PAGE);

  gtk_widget_class_set_template_from_resource (
    widget_class, "/org/unity/Greeter/unity-greeter.ui");
  gtk_widget_class_bind_template_child (widget_class, UnityGreeter, nav);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeter, cards);
}

static void
unity_greeter_init (UnityGreeter *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

UnityGreeter *
unity_greeter_new (GtkApplication *app,
                   GListModel     *users,
                   GListModel     *sessions)
{
  g_return_val_if_fail (GTK_IS_APPLICATION (app), NULL);
  g_return_val_if_fail (G_IS_LIST_MODEL (users), NULL);
  g_return_val_if_fail (G_IS_LIST_MODEL (sessions), NULL);

  UnityGreeter *self = g_object_new (UNITY_TYPE_GREETER,
                                     "application", app,
                                     NULL);
  self->users    = g_object_ref (users);
  self->sessions = g_object_ref (sessions);

  g_signal_connect_object (self->users, "items-changed",
                           G_CALLBACK (on_users_changed), self, G_CONNECT_DEFAULT);
  sync_cards (self);

  unity_greeter_idle_watch (GTK_WINDOW (self));

  return self;
}
