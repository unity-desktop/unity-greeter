/* unity-greeter-wallpaper.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_WALLPAPER (unity_greeter_wallpaper_get_type ())

G_DECLARE_FINAL_TYPE (UnityGreeterWallpaper, unity_greeter_wallpaper,
                      UNITY_GREETER, WALLPAPER, GObject)

UnityGreeterWallpaper *unity_greeter_wallpaper_new     (void);

void                   unity_greeter_wallpaper_publish (UnityGreeterWallpaper *self);

G_END_DECLS
