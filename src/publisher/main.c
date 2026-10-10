/* main.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gtk/gtk.h>

#include "unity-greeter-outputs.h"
#include "unity-greeter-wallpaper.h"

/* Runs in the user's session and keeps the files the greeter reads current. */

static void
publish (UnityGreeterWallpaper *wallpaper)
{
  unity_greeter_outputs_publish ();
  unity_greeter_wallpaper_publish (wallpaper);
}

static void
on_monitor_changed (GdkMonitor *monitor, GParamSpec *pspec, gpointer user_data)
{
  publish (user_data);
}

static void
on_monitors_changed (GListModel *monitors,
                     guint       position,
                     guint       removed,
                     guint       added,
                     gpointer    user_data)
{
  for (guint i = 0; i < g_list_model_get_n_items (monitors); i++)
    {
      g_autoptr (GdkMonitor) monitor = g_list_model_get_item (monitors, i);
      g_signal_handlers_disconnect_by_func (monitor, G_CALLBACK (on_monitor_changed), user_data);
      g_signal_connect (monitor, "notify::scale", G_CALLBACK (on_monitor_changed), user_data);
      g_signal_connect (monitor, "notify::geometry", G_CALLBACK (on_monitor_changed), user_data);
    }

  publish (user_data);
}

gint
main (void)
{
  gtk_init ();

  g_autoptr (UnityGreeterWallpaper) wallpaper = unity_greeter_wallpaper_new ();
  GListModel *monitors = gdk_display_get_monitors (gdk_display_get_default ());
  g_signal_connect (monitors, "items-changed", G_CALLBACK (on_monitors_changed), wallpaper);
  on_monitors_changed (monitors, 0, 0, 0, wallpaper);

  g_autoptr (GMainLoop) loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  return 0;
}
