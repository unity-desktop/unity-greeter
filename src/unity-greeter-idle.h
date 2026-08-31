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

#include <glib.h>

G_BEGIN_DECLS

/* Watches the seat for inactivity: powers every output off after 2
   minutes, asks logind to suspend after 3. The output step needs
   wlr-output-power-management-unstable-v1; the whole watcher needs
   ext-idle-notify-v1 and does nothing without it. Call once. */
void unity_greeter_idle_watch (void);

G_END_DECLS
