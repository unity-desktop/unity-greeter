/* unity-greeter.h
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

#include <adwaita.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define UNITY_TYPE_GREETER (unity_greeter_get_type ())

/* Fullscreen greeter shell surface. */
G_DECLARE_FINAL_TYPE (UnityGreeter, unity_greeter,
                      UNITY, GREETER, AdwApplicationWindow)

/* @users holds ActUser, @sessions holds UnityGreeterSession. */
UnityGreeter *unity_greeter_new (GtkApplication *app,
                                 GListModel     *users,
                                 GListModel     *sessions);

G_END_DECLS
