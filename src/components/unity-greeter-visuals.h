/* unity-greeter-visuals.h
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

#include "unity-greeter-user.h"

G_BEGIN_DECLS

void unity_greeter_apply_identity  (AdwAvatar        *avatar,
                                    GtkLabel         *name_label,
                                    ActUser *user);

void unity_greeter_apply_wallpaper (GtkPicture       *picture,
                                    ActUser *user,
                                    const gchar      *basename);

void unity_greeter_set_status_text (GtkLabel         *label,
                                    const gchar      *text,
                                    gboolean          is_error);

G_END_DECLS
