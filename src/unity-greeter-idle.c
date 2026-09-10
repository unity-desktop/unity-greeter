/* unity-greeter-idle.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "unity-greeter-idle.h"

#include <astal-idle-notify.h>
#include <astal-wl.h>
#include <libdex.h>
#include <unity-quit.h>

#define SUSPEND_TIMEOUT_MS 180000

static AstalIdleNotifyNotification *suspend_notification;

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

  AstalIdleNotifyNotifier *notifier = astal_idle_notify_get_default ();

  suspend_notification = astal_idle_notify_notifier_get_idle_notification_for_seat (
    notifier, SUSPEND_TIMEOUT_MS, seat);
  g_signal_connect (suspend_notification, "idled", G_CALLBACK (on_suspend_idled), NULL);
}
