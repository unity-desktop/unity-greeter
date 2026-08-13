#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
 * unity_greeter_idle_watch:
 * @window: the greeter's root window.
 *
 * Registers two idle notifications on the seat via `ext-idle-notify-v1`.
 * At 30 s of inactivity the window gets an `idle-dim` CSS class so a
 * translucent overlay fades in. At 2 min the compositor is asked to
 * power every output off via `wlr-output-power-management`. Any input
 * reverses both.
 *
 * A no-op on non-Wayland displays or on compositors that lack
 * `ext-idle-notify-v1`. The output-power step degrades gracefully if
 * that protocol is missing.
 */
void unity_greeter_idle_watch (GtkWindow *window);

G_END_DECLS
