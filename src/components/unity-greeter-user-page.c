/* unity-greeter-user-page.c
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

#include "unity-greeter-user-page.h"

#include <string.h>

#include <glib/gi18n.h>

#include "unity-greeter-conversation.h"
#include "unity-greeter-defs.h"
#include "unity-greeter-session-dialog.h"
#include "unity-greeter-session-list.h"

struct _UnityGreeterUserPage
{
  AdwNavigationPage parent_instance;

  GtkPicture          *wallpaper;
  AdwAvatar           *avatar;
  GtkLabel            *name_label;
  GtkLabel            *message;
  AdwPasswordEntryRow *entry;
  GtkLabel            *caps_warning;
  AdwButtonRow        *login_button;
  AdwViewStack        *stack;
  GtkButton           *session_button;
  GtkButton           *back;

  UnityGreeterUser         *user;
  GListModel               *sessions;
  UnityGreeterConversation *conversation;

  gchar    *selected_session;
  gchar    *pending;
  gboolean  started;
};

G_DEFINE_FINAL_TYPE (UnityGreeterUserPage, unity_greeter_user_page,
                     ADW_TYPE_NAVIGATION_PAGE)

static void
show_message (UnityGreeterUserPage *self,
              const gchar          *text,
              gboolean              is_error)
{
  gtk_label_set_text (self->message, text != NULL ? text : "");
  gtk_widget_set_visible (GTK_WIDGET (self->message),
                          text != NULL && *text != '\0');
  gtk_widget_remove_css_class (GTK_WIDGET (self->message), "error");
  gtk_widget_remove_css_class (GTK_WIDGET (self->message), "dim-label");
  gtk_widget_add_css_class    (GTK_WIDGET (self->message),
                               is_error ? "error" : "dim-label");
}

static void
set_busy (UnityGreeterUserPage *self, gboolean busy)
{
  adw_view_stack_set_visible_child_name (self->stack, busy ? "busy" : "form");
  gtk_widget_set_visible   (GTK_WIDGET (self->back),         !busy);
  gtk_widget_set_sensitive (GTK_WIDGET (self->entry),        !busy);
  gtk_widget_set_sensitive (GTK_WIDGET (self->login_button), !busy);
  if (!busy)
    gtk_widget_grab_focus (GTK_WIDGET (self->entry));
}

static const gchar *
button_label_for_prompt (const gchar *prompt)
{
  g_autofree gchar *lower =
    prompt != NULL ? g_ascii_strdown (prompt, -1) : g_strdup ("");

  if (strstr (lower, "retype") != NULL ||
      strstr (lower, "confirm") != NULL ||
      strstr (lower, "again") != NULL)
    return _("Confirm");
  if (strstr (lower, "new") != NULL || strstr (lower, "change") != NULL)
    return _("Set Password");
  return _("Login");
}

static UnityGreeterSession *
resolve_session (UnityGreeterUserPage *self)
{
  if (self->selected_session != NULL)
    {
      UnityGreeterSession *s =
        unity_greeter_session_list_find (self->sessions, self->selected_session);
      if (s != NULL)
        return g_object_ref (s);
    }
  if (g_list_model_get_n_items (self->sessions) > 0)
    return UNITY_GREETER_SESSION (g_list_model_get_item (self->sessions, 0));
  return NULL;
}

static void
sync_caps_warning (UnityGreeterUserPage *self)
{
  GdkDisplay *display = gdk_display_get_default ();
  GdkSeat    *seat    = display != NULL ? gdk_display_get_default_seat (display) : NULL;
  GdkDevice  *kbd     = seat    != NULL ? gdk_seat_get_keyboard (seat) : NULL;
  gboolean    caps    = kbd != NULL && gdk_device_get_caps_lock_state (kbd);
  gtk_widget_set_visible (GTK_WIDGET (self->caps_warning), caps);
}

static void
on_caps_lock_changed (GObject *o, GParamSpec *p, gpointer u)
{
  (void) o; (void) p;
  sync_caps_warning (UNITY_GREETER_USER_PAGE (u));
}

static void
submit (UnityGreeterUserPage *self)
{
  const gchar *text = gtk_editable_get_text (GTK_EDITABLE (self->entry));
  if (text == NULL)
    text = "";

  set_busy (self, TRUE);

  if (self->started)
    {
      unity_greeter_conversation_answer (self->conversation, text);
      return;
    }

  g_set_str (&self->pending, text);
  self->started = TRUE;
  unity_greeter_conversation_begin (self->conversation,
    unity_greeter_user_get_user_name (self->user));
}

static void on_login (AdwButtonRow *r, gpointer u) { (void) r; submit (u); }
static void on_enter (AdwEntryRow  *r, gpointer u) { (void) r; submit (u); }

static void
on_session_picked (UnityGreeterSessionDialog *dialog,
                   const gchar               *id,
                   gpointer                   user_data)
{
  (void) dialog;
  UnityGreeterUserPage *self = user_data;
  g_set_str (&self->selected_session, id);
}

static void
on_pick_session (GtkButton *b, gpointer user_data)
{
  (void) b;
  UnityGreeterUserPage *self = user_data;
  AdwDialog *dialog =
    unity_greeter_session_dialog_new (self->sessions, self->selected_session);
  g_signal_connect (dialog, "session-selected",
                    G_CALLBACK (on_session_picked), self);
  adw_dialog_present (dialog, GTK_WIDGET (self));
}

static void
on_prompt (UnityGreeterConversation *conv,
           const gchar              *label,
           gboolean                  secret,
           gpointer                  user_data)
{
  (void) conv;
  (void) secret;   /* our one entry is always masked */
  UnityGreeterUserPage *self = user_data;

  if (self->pending != NULL)
    {
      g_autofree gchar *r = g_steal_pointer (&self->pending);
      unity_greeter_conversation_answer (self->conversation, r);
      return;
    }

  g_autofree gchar *title = g_strdup (label != NULL ? label : "");
  g_strstrip (title);
  gsize n = strlen (title);
  while (n > 0 && title[n - 1] == ':')
    title[--n] = '\0';
  g_strstrip (title);

  const gchar *shown = *title != '\0' ? title : _("Password");
  adw_preferences_row_set_title (ADW_PREFERENCES_ROW (self->entry), shown);
  adw_preferences_row_set_title (ADW_PREFERENCES_ROW (self->login_button),
                                 button_label_for_prompt (shown));
  gtk_editable_set_text (GTK_EDITABLE (self->entry), "");

  set_busy (self, FALSE);
}

static void
on_message (UnityGreeterConversation *conv,
            const gchar              *text,
            gboolean                  is_error,
            gpointer                  user_data)
{
  (void) conv;
  show_message (UNITY_GREETER_USER_PAGE (user_data), text, is_error);
}

static void
on_authenticated (UnityGreeterConversation *conv, gpointer user_data)
{
  (void) conv;
  UnityGreeterUserPage *self = user_data;

  show_message (self, NULL, FALSE);
  set_busy (self, TRUE);

  if (self->selected_session != NULL)
    unity_greeter_user_persist_session (self->user, self->selected_session);

  g_autoptr (UnityGreeterSession) session = resolve_session (self);
  if (session == NULL)
    {
      show_message (self, _("No sessions installed"), TRUE);
      set_busy (self, FALSE);
      return;
    }

  g_autoptr (GError) error = NULL;
  g_auto (GStrv) argv = unity_greeter_session_parse_command (session, &error);
  if (argv == NULL)
    {
      g_warning ("session command malformed: %s", error->message);
      show_message (self, _("Session command is malformed"), TRUE);
      set_busy (self, FALSE);
      return;
    }
  g_auto (GStrv) env = unity_greeter_session_build_environment (session);

  unity_greeter_conversation_start (self->conversation,
    (const gchar * const *) argv, (const gchar * const *) env);
}

static void
on_failed (UnityGreeterConversation *conv, GError *error, gpointer user_data)
{
  (void) conv;
  UnityGreeterUserPage *self = user_data;

  show_message (self,
    error->code == UNITY_GREETER_ERROR_AUTH ? _("Incorrect password")
                                            : _("Login failed"),
    TRUE);
  adw_preferences_row_set_title (ADW_PREFERENCES_ROW (self->login_button),
                                 _("Login"));
  set_busy (self, FALSE);

  self->started = FALSE;
  g_clear_pointer (&self->pending, g_free);
}

static void
on_hidden (AdwNavigationPage *page, gpointer u)
{
  (void) u;
  unity_greeter_conversation_cancel (UNITY_GREETER_USER_PAGE (page)->conversation);
}

static void
on_shown (AdwNavigationPage *page, gpointer u)
{
  (void) u;
  UnityGreeterUserPage *self = UNITY_GREETER_USER_PAGE (page);
  if (g_strcmp0 (adw_view_stack_get_visible_child_name (self->stack), "form") == 0)
    gtk_widget_grab_focus (GTK_WIDGET (self->entry));
}

static void
apply_identity (UnityGreeterUserPage *self)
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
apply_wallpaper (UnityGreeterUserPage *self)
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
apply_password_hint (UnityGreeterUserPage *self)
{
  switch (unity_greeter_user_get_password_mode (self->user))
    {
    case ACT_USER_PASSWORD_MODE_SET_AT_LOGIN:
      show_message (self, _("New password required. Press Enter to continue."), FALSE);
      break;
    case ACT_USER_PASSWORD_MODE_NONE:
      show_message (self, _("This account has no password. Press Enter to sign in."), FALSE);
      break;
    default:
      break;
    }
}

static void
apply_session_defaults (UnityGreeterUserPage *self)
{
  gtk_widget_set_visible (GTK_WIDGET (self->session_button),
                          g_list_model_get_n_items (self->sessions) > 1);

  const gchar *last = unity_greeter_user_get_session (self->user);
  if (last != NULL && *last != '\0' &&
      unity_greeter_session_list_find (self->sessions, last) != NULL)
    {
      self->selected_session = g_strdup (last);
      return;
    }
  if (g_list_model_get_n_items (self->sessions) > 0)
    {
      g_autoptr (UnityGreeterSession) first =
        UNITY_GREETER_SESSION (g_list_model_get_item (self->sessions, 0));
      self->selected_session = g_strdup (unity_greeter_session_get_id (first));
    }
}

static void
unity_greeter_user_page_dispose (GObject *object)
{
  UnityGreeterUserPage *self = UNITY_GREETER_USER_PAGE (object);
  gtk_widget_dispose_template (GTK_WIDGET (self), UNITY_GREETER_TYPE_USER_PAGE);
  g_clear_object (&self->conversation);
  g_clear_object (&self->sessions);
  g_clear_object (&self->user);
  G_OBJECT_CLASS (unity_greeter_user_page_parent_class)->dispose (object);
}

static void
unity_greeter_user_page_finalize (GObject *object)
{
  UnityGreeterUserPage *self = UNITY_GREETER_USER_PAGE (object);
  g_clear_pointer (&self->selected_session, g_free);
  g_clear_pointer (&self->pending, g_free);
  G_OBJECT_CLASS (unity_greeter_user_page_parent_class)->finalize (object);
}

static void
unity_greeter_user_page_class_init (UnityGreeterUserPageClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose  = unity_greeter_user_page_dispose;
  object_class->finalize = unity_greeter_user_page_finalize;

  gtk_widget_class_set_template_from_resource (widget_class,
    "/org/unity/Greeter/unity-greeter-user-page.ui");
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserPage, wallpaper);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserPage, avatar);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserPage, name_label);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserPage, message);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserPage, entry);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserPage, caps_warning);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserPage, login_button);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserPage, stack);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserPage, session_button);
  gtk_widget_class_bind_template_child (widget_class, UnityGreeterUserPage, back);
}

static void
unity_greeter_user_page_init (UnityGreeterUserPage *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

AdwNavigationPage *
unity_greeter_user_page_new (UnityGreeterUser *user, GListModel *sessions)
{
  g_return_val_if_fail (UNITY_GREETER_IS_USER (user), NULL);
  g_return_val_if_fail (G_IS_LIST_MODEL (sessions), NULL);

  UnityGreeterUserPage *self = g_object_new (UNITY_GREETER_TYPE_USER_PAGE, NULL);
  self->user     = g_object_ref (user);
  self->sessions = g_object_ref (sessions);

  apply_identity (self);
  apply_wallpaper (self);
  apply_password_hint (self);
  apply_session_defaults (self);
  sync_caps_warning (self);

  g_signal_connect (self->entry,          "entry-activated", G_CALLBACK (on_enter),        self);
  g_signal_connect (self->login_button,   "activated",       G_CALLBACK (on_login),        self);
  g_signal_connect (self->session_button, "clicked",         G_CALLBACK (on_pick_session), self);
  g_signal_connect (self,                 "hidden",          G_CALLBACK (on_hidden),       NULL);
  g_signal_connect (self,                 "shown",           G_CALLBACK (on_shown),        NULL);

  GdkDisplay *display = gdk_display_get_default ();
  GdkSeat    *seat    = display != NULL ? gdk_display_get_default_seat (display) : NULL;
  GdkDevice  *kbd     = seat    != NULL ? gdk_seat_get_keyboard (seat) : NULL;
  if (kbd != NULL)
    g_signal_connect_object (kbd, "notify::caps-lock-state",
                             G_CALLBACK (on_caps_lock_changed), self, G_CONNECT_DEFAULT);

  self->conversation = unity_greeter_conversation_new ();
  g_signal_connect (self->conversation, "prompt",        G_CALLBACK (on_prompt),        self);
  g_signal_connect (self->conversation, "message",       G_CALLBACK (on_message),       self);
  g_signal_connect (self->conversation, "authenticated", G_CALLBACK (on_authenticated), self);
  g_signal_connect (self->conversation, "failed",        G_CALLBACK (on_failed),        self);

  set_busy (self, FALSE);

  return ADW_NAVIGATION_PAGE (self);
}
