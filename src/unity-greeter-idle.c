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
#include <astal-wl.h>
#include <libdex.h>
#include <unity-quit.h>
#include <unity-wlr-output-power.h>

#define BLANK_TIMEOUT_MS   120000   /* 2 min idle powers outputs off */
#define SUSPEND_TIMEOUT_MS 180000   /* 3 min idle asks logind to suspend */

static UnityWlrOutputPowerManager  *power_manager;
static GPtrArray                   *blanked_controls;
static AstalIdleNotifyNotification *blank_notification;
static AstalIdleNotifyNotification *suspend_notification;

static void
blank_outputs (void)
{
  if (power_manager == NULL)
    return;

  GList *outputs = astal_wl_registry_get_outputs (astal_wl_registry_get_default ());
  for (GList *l = outputs; l != NULL; l = l->next)
    {
      UnityWlrOutputPower *control =
        unity_wlr_output_power_manager_get_output_power (power_manager,
                                                         ASTAL_WL_OUTPUT (l->data));
      if (control == NULL)
        continue;

      unity_wlr_output_power_request_mode (control, UNITY_WLR_OUTPUT_POWER_MODE_OFF);
      g_ptr_array_add (blanked_controls, control);
    }
  g_list_free (outputs);
}

static void
unblank_outputs (void)
{
  for (guint i = 0; i < blanked_controls->len; i++)
    unity_wlr_output_power_request_mode (g_ptr_array_index (blanked_controls, i),
                                         UNITY_WLR_OUTPUT_POWER_MODE_ON);
  g_ptr_array_set_size (blanked_controls, 0);
}

static void
on_blank_idled (AstalIdleNotifyNotification *n, gpointer data)
{
  blank_outputs ();
}

static void
on_blank_resumed (AstalIdleNotifyNotification *n, gpointer data)
{
  unblank_outputs ();
}

static DexFuture *
on_suspend_replied (DexFuture *future, gpointer user_data)
{
  g_autoptr (GError) error = NULL;
  if (dex_future_get_value (future, &error) == NULL)
    g_warning ("idle: suspend failed: %s", error->message);
  return NULL;
}

static void
on_suspend_idled (AstalIdleNotifyNotification *n, gpointer data)
{
  DexFuture *future = unity_quit_suspend (unity_quit_get_default ());
  dex_future_disown (dex_future_finally (future, on_suspend_replied, NULL, NULL));
}

static void
on_prepare_for_sleep (UnityQuit *quit, gboolean start, gpointer data)
{
  if (!start)
    unblank_outputs ();
}

void
unity_greeter_idle_watch (void)
{
  if (!astal_idle_notify_is_supported ())
    {
      g_warning ("idle: compositor lacks ext-idle-notifier-v1, skipping");
      return;
    }

  GList       *seats = astal_wl_registry_get_seats (astal_wl_registry_get_default ());
  AstalWlSeat *seat  = seats != NULL ? seats->data : NULL;
  g_list_free (seats);

  if (seat == NULL)
    {
      g_warning ("idle: no wayland seat, skipping");
      return;
    }

  power_manager    = unity_wlr_output_power_manager_get_default ();
  blanked_controls = g_ptr_array_new_with_free_func (g_object_unref);

  AstalIdleNotifyNotifier *notifier = astal_idle_notify_get_default ();

  blank_notification = astal_idle_notify_notifier_get_idle_notification_for_seat (
    notifier, BLANK_TIMEOUT_MS, seat);
  if (power_manager != NULL)
    {
      g_signal_connect (blank_notification, "idled",   G_CALLBACK (on_blank_idled),   NULL);
      g_signal_connect (blank_notification, "resumed", G_CALLBACK (on_blank_resumed), NULL);
    }

  suspend_notification = astal_idle_notify_notifier_get_idle_notification_for_seat (
    notifier, SUSPEND_TIMEOUT_MS, seat);
  g_signal_connect (suspend_notification, "idled", G_CALLBACK (on_suspend_idled), NULL);

  g_signal_connect (unity_quit_get_default (), "prepare-for-sleep",
                    G_CALLBACK (on_prepare_for_sleep), NULL);
}
