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

#include <astal-idle-notify.h>
#include <astal-logind.h>
#include <astal-wl.h>
#include <astal-wlr.h>

#define BLANK_TIMEOUT_MS   120000   /* 2 min idle powers outputs off */
#define SUSPEND_TIMEOUT_MS 180000   /* 3 min idle asks logind to suspend */

typedef struct
{
  AstalWlrOutputPowerManager  *power_manager;
  GPtrArray                   *power_controls;
  AstalIdleNotifyNotification *blank;
  AstalIdleNotifyNotification *suspend;
} Idle;

static void
power_outputs (Idle *idle, AstalWlrOutputPowerMode mode)
{
  if (idle->power_manager == NULL)
    return;

  if (mode == ASTAL_WLR_OUTPUT_POWER_MODE_ON)
    {
      for (guint i = 0; i < idle->power_controls->len; i++)
        {
          AstalWlrOutputPower *p = g_ptr_array_index (idle->power_controls, i);
          astal_wlr_output_power_request_mode (p, ASTAL_WLR_OUTPUT_POWER_MODE_ON);
        }
      g_ptr_array_set_size (idle->power_controls, 0);
      return;
    }

  GList *outputs = astal_wl_registry_get_outputs (astal_wl_registry_get_default ());
  for (GList *l = outputs; l != NULL; l = l->next)
    {
      AstalWlrOutputPower *p =
        astal_wlr_output_power_manager_get_output_power (idle->power_manager,
                                                         ASTAL_WL_OUTPUT (l->data));
      if (p == NULL)
        continue;
      astal_wlr_output_power_request_mode (p, ASTAL_WLR_OUTPUT_POWER_MODE_OFF);
      g_ptr_array_add (idle->power_controls, p);
    }
  g_list_free (outputs);
}

static void
on_blank_idled (AstalIdleNotifyNotification *n, gpointer data)
{
  power_outputs (data, ASTAL_WLR_OUTPUT_POWER_MODE_OFF);
}

static void
on_blank_resumed (AstalIdleNotifyNotification *n, gpointer data)
{
  power_outputs (data, ASTAL_WLR_OUTPUT_POWER_MODE_ON);
}

static void
on_suspend_idled (AstalIdleNotifyNotification *n, gpointer data)
{
  AstalLogindLogind *logind = astal_logind_get_default ();
  if (logind == NULL)
    {
      g_warning ("idle: logind unavailable, skipping suspend");
      return;
    }
  astal_logind_logind_suspend (logind);
}

void
unity_greeter_idle_watch (void)
{
  /* Pre-warm astal-logind so can_suspend has settled by the time the
     3-minute notification fires. */
  astal_logind_get_default ();

  /* astal-idle-notify aborts on a compositor without the protocol, so
     ask before touching the notifier. */
  if (!astal_idle_notify_is_supported ())
    {
      g_warning ("idle: compositor lacks ext-idle-notifier-v1, skipping");
      return;
    }

  /* The list holds weak references, so freeing it leaves the seat alone. */
  GList       *seats = astal_wl_registry_get_seats (astal_wl_registry_get_default ());
  AstalWlSeat *seat  = seats != NULL ? seats->data : NULL;
  g_list_free (seats);

  if (seat == NULL)
    {
      g_warning ("idle: no wayland seat, skipping");
      return;
    }

  /* The watcher runs for the life of the process, so its state is static
     rather than heap-allocated and never freed. */
  static Idle idle;

  idle.power_manager  = astal_wlr_output_power_manager_get_default ();
  idle.power_controls = g_ptr_array_new_with_free_func (g_object_unref);

  AstalIdleNotifyNotifier *notifier = astal_idle_notify_get_default ();

  /* Each notification owns its wayland listener: dropping the object
     stops the signals, so both are kept. */
  idle.blank =
    astal_idle_notify_notifier_get_idle_notification_for_seat (notifier,
                                                               BLANK_TIMEOUT_MS,
                                                               seat);
  if (idle.power_manager != NULL)
    {
      g_signal_connect (idle.blank, "idled",   G_CALLBACK (on_blank_idled),   &idle);
      g_signal_connect (idle.blank, "resumed", G_CALLBACK (on_blank_resumed), &idle);
    }

  idle.suspend =
    astal_idle_notify_notifier_get_idle_notification_for_seat (notifier,
                                                               SUSPEND_TIMEOUT_MS,
                                                               seat);
  g_signal_connect (idle.suspend, "idled", G_CALLBACK (on_suspend_idled), &idle);
}
