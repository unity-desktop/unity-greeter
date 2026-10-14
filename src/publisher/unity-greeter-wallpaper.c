/* unity-greeter-wallpaper.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "unity-greeter-wallpaper.h"

#include <gtk/gtk.h>
#include <unity-backgrounds-source.h>

#include "unity-greeter-publish.h"

#define BLUR  60     
#define SCALE 0.25

static UnityBackgroundsSource *source;

static void
largest_monitor (GdkRectangle *screen)
{
  GListModel *monitors = gdk_display_get_monitors (gdk_display_get_default ());

  *screen = (GdkRectangle) { 0, 0, 0, 0 };
  for (guint i = 0; i < g_list_model_get_n_items (monitors); i++)
    {
      g_autoptr (GdkMonitor) monitor = g_list_model_get_item (monitors, i);
      GdkRectangle geometry;
      gdk_monitor_get_geometry (monitor, &geometry);
      if (geometry.width * geometry.height > screen->width * screen->height)
        *screen = (GdkRectangle) { 0, 0, geometry.width, geometry.height };
    }
}

void
unity_greeter_wallpaper_publish (void)
{
  GdkRectangle screen;
  largest_monitor (&screen);
  gint width  = screen.width * SCALE;
  gint height = screen.height * SCALE;
  gdouble radius = BLUR * SCALE;

  g_autoptr (GdkTexture) thumbnail =
    unity_backgrounds_source_create_thumbnail (source, &screen, width, height);
  if (thumbnail == NULL)
    {
      g_warning ("wallpaper: no thumbnail");
      return;
    }

  GtkSnapshot *snapshot = gtk_snapshot_new ();
  gtk_snapshot_push_blur (snapshot, radius);
  gtk_snapshot_translate (snapshot, &GRAPHENE_POINT_INIT (-radius, -radius));
  gdk_paintable_snapshot (GDK_PAINTABLE (thumbnail), snapshot,
                          width + 2 * radius, height + 2 * radius);
  gtk_snapshot_pop (snapshot);
  g_autoptr (GskRenderNode) node = gtk_snapshot_free_to_node (snapshot);

  g_autoptr (GskRenderer) renderer = gsk_cairo_renderer_new ();
  gsk_renderer_realize_for_display (renderer, gdk_display_get_default (), NULL);
  g_autoptr (GdkTexture) blurred =
    gsk_renderer_render_texture (renderer, node, &GRAPHENE_RECT_INIT (0, 0, width, height));
  gsk_renderer_unrealize (renderer);

  g_autoptr (GBytes) png = gdk_texture_save_to_png_bytes (blurred);
  g_autoptr (GError) error = NULL;
  if (!unity_greeter_publish_bytes (UNITY_GREETER_PUBLISH_WALLPAPER, png, &error))
    g_warning ("wallpaper: %s", error->message);
}

static void
on_source_changed (GdkPaintable *paintable, gpointer user_data)
{
  unity_greeter_wallpaper_publish ();
}

void
unity_greeter_wallpaper_watch (void)
{
  source = unity_backgrounds_source_new ();
  g_signal_connect (source, "invalidate-contents", G_CALLBACK (on_source_changed), NULL);
}
