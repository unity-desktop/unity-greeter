/* unity-greeter-idle.h
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
