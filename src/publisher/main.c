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
on_monitor_changed (GdkMonitor *monitor, GParamSpec *pspec, gpointer user_data)
{
  unity_greeter_outputs_publish ();
  unity_greeter_wallpaper_publish ();
}

static void
watch_monitors (GListModel *monitors, guint position, guint added)
{
  for (guint i = position; i < position + added; i++)
    {
      g_autoptr (GdkMonitor) monitor = g_list_model_get_item (monitors, i);
      g_signal_connect (monitor, "notify::scale", G_CALLBACK (on_monitor_changed), NULL);
      g_signal_connect (monitor, "notify::geometry", G_CALLBACK (on_monitor_changed), NULL);
    }
}

static void
on_monitors_changed (GListModel *monitors,
                     guint       position,
                     guint       removed,
                     guint       added,
                     gpointer    user_data)
{
  watch_monitors (monitors, position, added);
  on_monitor_changed (NULL, NULL, NULL);
}

gint
main (void)
{
  gtk_init ();

  unity_greeter_wallpaper_watch ();
  unity_greeter_outputs_publish ();

  GListModel *monitors = gdk_display_get_monitors (gdk_display_get_default ());
  watch_monitors (monitors, 0, g_list_model_get_n_items (monitors));
  g_signal_connect (monitors, "items-changed", G_CALLBACK (on_monitors_changed), NULL);

  g_autoptr (GMainLoop) loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  return 0;
}
