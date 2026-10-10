/* unity-greeter-wallpaper.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "unity-greeter-wallpaper.h"

#include <unity-backgrounds-source.h>

#include "unity-greeter-publish.h"

#define BLUR 60

struct _UnityGreeterWallpaper
{
  GObject parent_instance;

  UnityBackgroundsSource *source;
  GskRenderer            *renderer;
  gboolean                saving;
  gboolean                stale;
};

G_DEFINE_FINAL_TYPE (UnityGreeterWallpaper, unity_greeter_wallpaper, G_TYPE_OBJECT)

static void
largest_monitor (gint *width, gint *height)
{
  GListModel *monitors = gdk_display_get_monitors (gdk_display_get_default ());

  *width  = 0;
  *height = 0;
  for (guint i = 0; i < g_list_model_get_n_items (monitors); i++)
    {
      g_autoptr (GdkMonitor) monitor = g_list_model_get_item (monitors, i);
      GdkRectangle geometry;
      gdouble      scale = gdk_monitor_get_scale (monitor);
      gdk_monitor_get_geometry (monitor, &geometry);

      gint w = (gint) (geometry.width * scale + 0.5);
      gint h = (gint) (geometry.height * scale + 0.5);

      if (w * h > *width * *height)
        {
          *width  = w;
          *height = h;
        }
    }
}

static GdkTexture *
render_blurred (UnityGreeterWallpaper *self, gint width, gint height, gint radius)
{
  GtkSnapshot *snapshot = gtk_snapshot_new ();
  gtk_snapshot_push_blur (snapshot, radius);
  gtk_snapshot_translate (snapshot, &GRAPHENE_POINT_INIT (-radius, -radius));
  gdk_paintable_snapshot (GDK_PAINTABLE (self->source), snapshot,
                          width + 2 * radius, height + 2 * radius);
  gtk_snapshot_pop (snapshot);
  g_autoptr (GskRenderNode) node = gtk_snapshot_free_to_node (snapshot);

  GdkTexture *drawn = unity_backgrounds_source_get_texture (self->source);
  if (drawn == NULL || gdk_texture_get_width (drawn) < width + 2 * radius)
    return NULL;

  return gsk_renderer_render_texture (self->renderer, node,
                                     &GRAPHENE_RECT_INIT (0, 0, width, height));
}

static void
save_worker (GTask        *task,
             gpointer      source_object,
             gpointer      task_data,
             GCancellable *cancellable)
{
  g_autoptr (GBytes) png = gdk_texture_save_to_png_bytes (task_data);
  GError *error = NULL;

  if (unity_greeter_publish_bytes (UNITY_GREETER_PUBLISH_WALLPAPER, png, &error))
    g_task_return_boolean (task, TRUE);
  else
    g_task_return_error (task, error);
}

static void
on_saved (GObject *source, GAsyncResult *result, gpointer user_data)
{
  UnityGreeterWallpaper *self = UNITY_GREETER_WALLPAPER (source);
  g_autoptr (GError) error = NULL;

  self->saving = FALSE;

  if (!g_task_propagate_boolean (G_TASK (result), &error))
    g_warning ("wallpaper: %s", error->message);

  if (self->stale)
    {
      self->stale = FALSE;
      unity_greeter_wallpaper_publish (self);
    }
}

static void
on_source_texture (UnityBackgroundsSource *source, GParamSpec *pspec, gpointer user_data)
{
  unity_greeter_wallpaper_publish (user_data);
}

static void
unity_greeter_wallpaper_dispose (GObject *object)
{
  UnityGreeterWallpaper *self = UNITY_GREETER_WALLPAPER (object);

  g_clear_object (&self->source);
  if (self->renderer != NULL)
    gsk_renderer_unrealize (self->renderer);
  g_clear_object (&self->renderer);

  G_OBJECT_CLASS (unity_greeter_wallpaper_parent_class)->dispose (object);
}

static void
unity_greeter_wallpaper_class_init (UnityGreeterWallpaperClass *klass)
{
  G_OBJECT_CLASS (klass)->dispose = unity_greeter_wallpaper_dispose;
}

static void
unity_greeter_wallpaper_init (UnityGreeterWallpaper *self)
{
  self->renderer = gsk_gl_renderer_new ();
  gsk_renderer_realize_for_display (self->renderer, gdk_display_get_default (), NULL);

  self->source = unity_backgrounds_source_new ();
  g_signal_connect_object (self->source, "notify::texture",
                           G_CALLBACK (on_source_texture), self, G_CONNECT_DEFAULT);
}

/**
 * unity_greeter_wallpaper_new:
 *
 * Creates the wallpaper publisher for the current user and publishes once.
 *
 * Returns: (transfer full): the new wallpaper publisher
 */
UnityGreeterWallpaper *
unity_greeter_wallpaper_new (void)
{
  UnityGreeterWallpaper *self = g_object_new (UNITY_GREETER_TYPE_WALLPAPER, NULL);
  unity_greeter_wallpaper_publish (self);
  return self;
}

/**
 * unity_greeter_wallpaper_publish:
 * @self: a #UnityGreeterWallpaper
 *
 * Renders and writes background.png. A call during a write runs
 * once more after it.
 */
void
unity_greeter_wallpaper_publish (UnityGreeterWallpaper *self)
{
  g_return_if_fail (UNITY_GREETER_IS_WALLPAPER (self));

  if (self->saving)
    {
      self->stale = TRUE;
      return;
    }

  gint width, height;
  largest_monitor (&width, &height);

  GdkTexture *texture = render_blurred (self, width, height, BLUR);
  if (texture == NULL)
    return;

  self->saving = TRUE;
  g_autoptr (GTask) task = g_task_new (self, NULL, on_saved, NULL);
  g_task_set_source_tag (task, unity_greeter_wallpaper_publish);
  g_task_set_task_data (task, texture, g_object_unref);
  g_task_run_in_thread (task, save_worker);
}
