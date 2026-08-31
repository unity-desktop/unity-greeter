/* unity-greeter-strength.h
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

/* Returns one localised line about @password: the pwquality improvement
   hint when it is rejected, otherwise "Password strength: <band>".
   Returns NULL for an empty password. @out_level runs 0 (empty) to 5. */
gchar *unity_greeter_strength_check (const gchar *password,
                                     const gchar *username,
                                     gboolean    *out_is_error,
                                     gint        *out_level);

G_END_DECLS
