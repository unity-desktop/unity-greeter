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

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * UNITY_GREETER_TYPE_STRENGTH:
 *
 * Type identifier for #UnityGreeterStrength.
 */
#define UNITY_GREETER_TYPE_STRENGTH (unity_greeter_strength_get_type ())

/**
 * UnityGreeterStrength:
 *
 * Wraps libpwquality. Loads the system configuration once and answers
 * strength queries about candidate passwords.
 */
G_DECLARE_FINAL_TYPE (UnityGreeterStrength, unity_greeter_strength,
                      UNITY_GREETER, STRENGTH, GObject)

/**
 * unity_greeter_strength_new:
 *
 * Loads the system pwquality configuration.
 *
 * Returns: (transfer full): a new #UnityGreeterStrength.
 */
UnityGreeterStrength *unity_greeter_strength_new (void);

/**
 * unity_greeter_strength_check:
 * @self: a #UnityGreeterStrength.
 * @password: candidate password.
 * @username: (nullable): the account name (used to reject passwords
 *   containing the user's name).
 * @out_hint: (out) (transfer none): localised improvement hint,
 *   e.g. "Try to use more numbers".
 * @out_level: (out): score band from 0 (empty) through 5 (excellent).
 * @out_level_hint: (out) (transfer none): localised label for the level,
 *   e.g. "weak", "strong".
 */
void unity_greeter_strength_check (UnityGreeterStrength *self,
                                   const gchar          *password,
                                   const gchar          *username,
                                   const gchar         **out_hint,
                                   gint                 *out_level,
                                   const gchar         **out_level_hint);

G_END_DECLS
