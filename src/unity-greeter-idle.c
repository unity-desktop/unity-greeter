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
#include <astal-wl.h>
#include <astal-wlr.h>

#define BLANK_TIMEOUT_MS   120000   /* 2 min idle powers outputs off */
#define SUSPEND_TIMEOUT_MS 180000   /* 3 min idle asks logind to suspend */

typedef struct
{
  AstalWlrOutputPowerManager *power_manager;
  GPtrArray                  *power_controls;
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
on_blank_idled (AstalWlIdleNotification *n, gpointer data)
{
  (void) n;
  power_outputs (data, ASTAL_WLR_OUTPUT_POWER_MODE_OFF);
}

static void
on_blank_resumed (AstalWlIdleNotification *n, gpointer data)
{
  (void) n;
  power_outputs (data, ASTAL_WLR_OUTPUT_POWER_MODE_ON);
}

static void
on_suspend_idled (AstalWlIdleNotification *n, gpointer data)
{
  (void) n;
  (void) data;

  AstalLogindLogind *logind = astal_logind_get_default ();
  if (logind == NULL)
    {
      g_warning ("idle: logind unavailable, skipping suspend");
      return;
    }
  astal_logind_logind_suspend (logind);
}

void
unity_greeter_idle_watch (GtkWindow *window)
{
  g_return_if_fail (GTK_IS_WINDOW (window));

  /* Pre-warm astal-logind so can_suspend has settled by the time the
     3-minute notification fires. */
  astal_logind_get_default ();

  AstalWlIdleNotifier *notifier = astal_wl_idle_notifier_get_default ();
  if (notifier == NULL)
    {
      g_warning ("idle: compositor lacks ext-idle-notify-v1, skipping");
      return;
    }

  Idle *idle = g_new0 (Idle, 1);
  idle->power_manager  = astal_wlr_output_power_manager_get_default ();
  idle->power_controls = g_ptr_array_new_with_free_func (g_object_unref);

  AstalWlIdleNotification *blank_noti =
    astal_wl_idle_notifier_get_notification (notifier, BLANK_TIMEOUT_MS);
  if (blank_noti != NULL && idle->power_manager != NULL)
    {
      g_signal_connect (blank_noti, "idled",   G_CALLBACK (on_blank_idled),   idle);
      g_signal_connect (blank_noti, "resumed", G_CALLBACK (on_blank_resumed), idle);
    }

  AstalWlIdleNotification *suspend_noti =
    astal_wl_idle_notifier_get_notification (notifier, SUSPEND_TIMEOUT_MS);
  if (suspend_noti != NULL)
    g_signal_connect (suspend_noti, "idled", G_CALLBACK (on_suspend_idled), idle);
}
