/* unity-greeter-strength.h
 *
 * Copyright 2026 Muqtadir
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
