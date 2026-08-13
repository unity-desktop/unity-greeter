/* unity-greeter-idle.c
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

#include "unity-greeter-idle.h"

#include <astal-logind.h>
#include <gdk/wayland/gdkwayland.h>

#include "ext-idle-notify-v1-client-protocol.h"
#include "wlr-output-power-management-unstable-v1-client-protocol.h"

#define BLANK_TIMEOUT_MS   120000   /* 2 min idle powers outputs off */
#define SUSPEND_TIMEOUT_MS 180000   /* 3 min idle asks logind to suspend */

typedef struct
{
  GtkWindow *window;

  struct wl_registry                  *registry;
  struct wl_seat                      *seat;
  struct ext_idle_notifier_v1         *idle_notifier;
  struct zwlr_output_power_manager_v1 *power_manager;

  struct ext_idle_notification_v1 *blank_noti;
  struct ext_idle_notification_v1 *suspend_noti;

  GPtrArray *power_controls;
} Idle;

static void
power_outputs (Idle *idle, enum zwlr_output_power_v1_mode mode)
{
  if (idle->power_manager == NULL)
    return;

  if (mode == ZWLR_OUTPUT_POWER_V1_MODE_ON)
    {
      for (guint i = 0; i < idle->power_controls->len; i++)
        {
          struct zwlr_output_power_v1 *p = g_ptr_array_index (idle->power_controls, i);
          zwlr_output_power_v1_set_mode (p, ZWLR_OUTPUT_POWER_V1_MODE_ON);
          zwlr_output_power_v1_destroy (p);
        }
      g_ptr_array_set_size (idle->power_controls, 0);
      return;
    }

  GdkDisplay *display  = gtk_widget_get_display (GTK_WIDGET (idle->window));
  GListModel *monitors = gdk_display_get_monitors (display);
  guint       n        = g_list_model_get_n_items (monitors);
  for (guint i = 0; i < n; i++)
    {
      g_autoptr (GdkMonitor) mon = g_list_model_get_item (monitors, i);
      struct wl_output *out = gdk_wayland_monitor_get_wl_output (mon);
      if (out == NULL)
        continue;

      struct zwlr_output_power_v1 *p =
        zwlr_output_power_manager_v1_get_output_power (idle->power_manager, out);
      g_ptr_array_add (idle->power_controls, p);
      zwlr_output_power_v1_set_mode (p, ZWLR_OUTPUT_POWER_V1_MODE_OFF);
    }
}

static void
on_blank_idled (void *data, struct ext_idle_notification_v1 *n)
{
  (void) n;
  power_outputs (data, ZWLR_OUTPUT_POWER_V1_MODE_OFF);
}

static void
on_blank_resumed (void *data, struct ext_idle_notification_v1 *n)
{
  (void) n;
  power_outputs (data, ZWLR_OUTPUT_POWER_V1_MODE_ON);
}

static const struct ext_idle_notification_v1_listener blank_listener = {
  .idled   = on_blank_idled,
  .resumed = on_blank_resumed,
};

static void
on_suspend_idled (void *data, struct ext_idle_notification_v1 *n)
{
  (void) data;
  (void) n;

  AstalLogindLogind *logind = astal_logind_get_default ();
  if (logind == NULL)
    {
      g_warning ("idle: logind unavailable, skipping suspend");
      return;
    }
  astal_logind_logind_suspend (logind);
}

static void
on_suspend_resumed (void *data, struct ext_idle_notification_v1 *n)
{
  (void) data;
  (void) n;
}

static const struct ext_idle_notification_v1_listener suspend_listener = {
  .idled   = on_suspend_idled,
  .resumed = on_suspend_resumed,
};

static void
on_registry_global (void               *data,
                    struct wl_registry *registry,
                    guint32            name,
                    const gchar         *interface,
                    guint32            version)
{
  Idle *idle = data;

  if (g_strcmp0 (interface, ext_idle_notifier_v1_interface.name) == 0)
    idle->idle_notifier = wl_registry_bind (registry, name,
      &ext_idle_notifier_v1_interface, MIN (version, 2u));
  else if (g_strcmp0 (interface, zwlr_output_power_manager_v1_interface.name) == 0)
    idle->power_manager = wl_registry_bind (registry, name,
      &zwlr_output_power_manager_v1_interface, 1);
  else if (g_strcmp0 (interface, wl_seat_interface.name) == 0 && idle->seat == NULL)
    idle->seat = wl_registry_bind (registry, name,
      &wl_seat_interface, MIN (version, 5u));
}

static void
on_registry_global_remove (void *data, struct wl_registry *r, guint32 name)
{
  (void) data; (void) r; (void) name;
}

static const struct wl_registry_listener registry_listener = {
  .global        = on_registry_global,
  .global_remove = on_registry_global_remove,
};

void
unity_greeter_idle_watch (GtkWindow *window)
{
  g_return_if_fail (GTK_IS_WINDOW (window));

  GdkDisplay *display = gtk_widget_get_display (GTK_WIDGET (window));
  if (!GDK_IS_WAYLAND_DISPLAY (display))
    return;

  astal_logind_get_default ();

  struct wl_display *wl = gdk_wayland_display_get_wl_display (display);

  Idle *idle = g_new0 (Idle, 1);
  idle->window         = window;
  idle->power_controls = g_ptr_array_new ();

  idle->registry = wl_display_get_registry (wl);
  wl_registry_add_listener (idle->registry, &registry_listener, idle);
  wl_display_roundtrip (wl);

  if (idle->idle_notifier == NULL || idle->seat == NULL)
    {
      g_warning ("idle: compositor lacks ext-idle-notify-v1, skipping");
      return;
    }

  if (idle->power_manager != NULL)
    {
      idle->blank_noti = ext_idle_notifier_v1_get_idle_notification (
        idle->idle_notifier, BLANK_TIMEOUT_MS, idle->seat);
      ext_idle_notification_v1_add_listener (idle->blank_noti, &blank_listener, idle);
    }

  idle->suspend_noti = ext_idle_notifier_v1_get_idle_notification (
    idle->idle_notifier, SUSPEND_TIMEOUT_MS, idle->seat);
  ext_idle_notification_v1_add_listener (idle->suspend_noti, &suspend_listener, idle);
}
