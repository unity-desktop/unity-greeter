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
 * @username: (nullable): the account name.
 * @out_is_error: (out): %TRUE when the returned message is a rejection.
 * @out_level: (out): score band from 0 (empty) through 5 (excellent).
 *
 * Returns a single localised line describing the candidate. On a
 * pwquality rejection the line is the improvement hint. Otherwise it
 * reads "Password strength: weak" (or strong, excellent, and so on).
 * Returns %NULL when @password is empty.
 *
 * Returns: (nullable) (transfer full): a newly allocated message.
 */
gchar *unity_greeter_strength_check (UnityGreeterStrength *self,
                                     const gchar          *password,
                                     const gchar          *username,
                                     gboolean             *out_is_error,
                                     gint                 *out_level);

G_END_DECLS
