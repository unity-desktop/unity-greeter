/* main.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <locale.h>

#include <adwaita.h>
#include <gio/gio.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <unity-window.h>

#include "unity-greeter-appearance.h"
#include "unity-greeter-idle.h"
#include "unity-greeter-scale.h"
#include "unity-greeter-session-list.h"
#include "unity-greeter-user.h"
#include "unity-greeter.h"

static GListModel *users_model;
static GListModel *sessions_model;

static void
push_optimal_outputs (GdkDisplay *display)
{
  if (unity_greeter_scale_other_user_published ())
    return;

  GListModel *monitors = display != NULL
                           ? gdk_display_get_monitors (display) : NULL;
  guint n = monitors != NULL ? g_list_model_get_n_items (monitors) : 0;
  if (n == 0)
    return;

  g_autoptr (GString) out = g_string_new (
    "# Auto-computed by unity-greeter replaced once unity-shell writes one.\n");

  gboolean wrote_any = FALSE;
  for (guint i = 0; i < n; i++)
    {
      g_autoptr (GdkMonitor) monitor = g_list_model_get_item (monitors, i);
      const gchar *connector = gdk_monitor_get_connector (monitor);
      if (connector == NULL)
        continue;

      gdouble scale = unity_window_compute_optimal_scale (monitor);
      g_string_append_printf (out, "\n[output:%s]\nscale = %.6f\n",
                              connector, scale);
      wrote_any = TRUE;
    }

  if (!wrote_any)
    return;

  g_autofree gchar *dir = g_build_filename (UNITY_GREETER_MIRROR_ROOT,
                                            g_get_user_name (), NULL);
  g_mkdir_with_parents (dir, 0755);
  g_autofree gchar *path = g_build_filename (dir, "outputs.ini", NULL);
  if (!g_file_set_contents (path, out->str, -1, NULL))
    return;
  g_chmod (path, 0644);

  g_autofree gchar *composed = unity_greeter_scale_compose ();
  if (composed == NULL)
    g_warning ("scale: recompose of wayfire.ini failed");
}

static void
on_startup (GApplication *app, gpointer user_data)
{
  unity_greeter_appearance_apply ();

  sessions_model = unity_greeter_session_list_new ();
  users_model    = unity_greeter_users_new ();

  push_optimal_outputs (gdk_display_get_default ());
  unity_greeter_idle_watch ();
}

static UnityWindow *
greeter_factory (GdkMonitor *monitor, gpointer user_data)
{
  return UNITY_WINDOW (unity_greeter_new (GTK_APPLICATION (user_data),
                                          users_model, sessions_model));
}

static void
on_activate (GApplication *app, gpointer user_data)
{
  unity_window_present_for_each_monitor (GTK_APPLICATION (app),
                                         greeter_factory, app, NULL);
}

gint
main (void)
{
  g_log_set_writer_func (g_log_writer_journald, NULL, NULL);

  setlocale (LC_ALL, "");
  bindtextdomain (GETTEXT_PACKAGE, UNITY_GREETER_LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  g_set_application_name (_("Unity Greeter"));

  g_autoptr (AdwApplication) app =
    adw_application_new ("org.unity.greeter", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect_after (app, "startup",  G_CALLBACK (on_startup),  NULL);
  g_signal_connect       (app, "activate", G_CALLBACK (on_activate), NULL);

  return g_application_run (G_APPLICATION (app), 0, NULL);
}
