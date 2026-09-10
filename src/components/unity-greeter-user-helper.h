/* unity-greeter-user-helper.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <adwaita.h>

#include "unity-greeter-user.h"

G_BEGIN_DECLS

void unity_greeter_apply_identity  (AdwAvatar   *avatar,
                                    GtkLabel    *name_label,
                                    ActUser     *user);

void unity_greeter_apply_wallpaper (GtkPicture  *picture,
                                    ActUser     *user,
                                    const gchar *basename);

void unity_greeter_set_status_text (GtkLabel    *label,
                                    const gchar *text,
                                    gboolean     is_error);

G_END_DECLS
