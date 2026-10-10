/* unity-greeter-publish.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <glib.h>

G_BEGIN_DECLS

#define UNITY_GREETER_PUBLISH_WALLPAPER "background.png"
#define UNITY_GREETER_PUBLISH_OUTPUTS   "outputs.ini"

gchar    *unity_greeter_publish_path  (const gchar *user_name,
                                       const gchar *basename);

gboolean  unity_greeter_publish_bytes (const gchar *basename,
                                       GBytes      *contents,
                                       GError     **error);

G_END_DECLS
