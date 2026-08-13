/* main.c
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

#include <locale.h>

#include <adwaita.h>
#include <gio/gio.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "unity-greeter-session-list.h"
#include "unity-greeter-user.h"
#include "unity-greeter.h"

#define INTERFACE_SCHEMA "org.gnome.desktop.interface"

typedef struct
{
  GListModel *users;
  GListModel *sessions;
} AppState;

static void
apply_string_key (GSettings   *settings,
                  const gchar *gsettings_key,
                  const gchar *gtk_property)
{
  g_autofree gchar *value = g_settings_get_string (settings, gsettings_key);
  if (value == NULL || *value == '\0')
    return;

  GtkSettings *gtk_settings = gtk_settings_get_default ();
  if (gtk_settings != NULL)
    g_object_set (gtk_settings, gtk_property, value, NULL);
}

static void
apply_appearance (void)
{
  GSettingsSchemaSource *source = g_settings_schema_source_get_default ();
  g_autoptr (GSettingsSchema) schema =
    source != NULL
      ? g_settings_schema_source_lookup (source, INTERFACE_SCHEMA, TRUE)
      : NULL;

  if (schema == NULL)
    {
      g_warning ("%s not installed, skipping appearance", INTERFACE_SCHEMA);
      return;
    }

  g_autoptr (GSettings) settings = g_settings_new_full (schema, NULL, NULL);

  apply_string_key (settings, "font-name",    "gtk-font-name");
  apply_string_key (settings, "icon-theme",   "gtk-icon-theme-name");
  apply_string_key (settings, "cursor-theme", "gtk-cursor-theme-name");

  gint cursor_size = g_settings_get_int (settings, "cursor-size");
  if (cursor_size > 0)
    g_object_set (gtk_settings_get_default (),
                  "gtk-cursor-theme-size", cursor_size,
                  NULL);

  adw_style_manager_set_color_scheme (adw_style_manager_get_default (),
                                      ADW_COLOR_SCHEME_FORCE_DARK);
}

static void
on_startup (GApplication *app, gpointer user_data)
{
  (void) app;
  AppState *state = user_data;

  apply_appearance ();

  state->sessions = unity_greeter_session_list_new ();
  state->users    = unity_greeter_users_new ();
}

static void
on_activate (GApplication *app, gpointer user_data)
{
  AppState *state = user_data;

  UnityGreeter *window = unity_greeter_new (GTK_APPLICATION (app),
                                            state->users, state->sessions);
  gtk_window_present (GTK_WINDOW (window));
}

gint
main (gint argc, gchar *argv[])
{
  (void) argc;

  g_log_set_writer_func (g_log_writer_journald, NULL, NULL);

  setlocale (LC_ALL, "");
  bindtextdomain (GETTEXT_PACKAGE, UNITY_GREETER_LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  g_set_application_name (_("Unity Greeter"));

  AppState state = { 0 };

  g_autoptr (AdwApplication) app = g_object_new (ADW_TYPE_APPLICATION,
    "application-id",     "org.unity.Greeter",
    "resource-base-path", "/org/unity/Greeter",
    NULL);
  g_signal_connect (app, "startup",  G_CALLBACK (on_startup),  &state);
  g_signal_connect (app, "activate", G_CALLBACK (on_activate), &state);

  gchar *forwarded[] = { argv[0], NULL };
  gint rc = g_application_run (G_APPLICATION (app), 1, forwarded);

  g_clear_object (&state.users);
  g_clear_object (&state.sessions);
  return rc;
}
