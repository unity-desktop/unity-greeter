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
 * Starts watching the seat for inactivity and reacts in two steps.
 *
 * After 2 minutes of no input the greeter asks the compositor to power
 * every output off. Any input turns the outputs back on.
 *
 * After 3 minutes of no input the greeter asks logind to suspend the
 * machine.
 *
 * The output step needs the compositor to support
 * `wlr-output-power-management-unstable-v1`. On a compositor without
 * this protocol the greeter still suspends after 3 minutes but the
 * outputs stay on. The whole watcher does nothing on a compositor
 * without `ext-idle-notify-v1`.
 */
void unity_greeter_idle_watch (GtkWindow *window);

G_END_DECLS
