/* unity-greeter-appearance.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "unity-greeter-appearance.h"

#include <adwaita.h>

#define INTERFACE_SCHEMA "org.gnome.desktop.interface"

static const struct
{
  const gchar *key;
  const gchar *property;
} string_keys[] = {
  { "font-name",    "gtk-font-name"         },
  { "icon-theme",   "gtk-icon-theme-name"   },
  { "cursor-theme", "gtk-cursor-theme-name" },
};

void
unity_greeter_appearance_apply (void)
{
  g_autoptr (GSettings) settings = g_settings_new (INTERFACE_SCHEMA);
  GtkSettings *gtk_settings = gtk_settings_get_default ();

  for (gsize i = 0; i < G_N_ELEMENTS (string_keys); i++)
    {
      g_autofree gchar *value =
        g_settings_get_string (settings, string_keys[i].key);

      if (*value != '\0')
        g_object_set (gtk_settings, string_keys[i].property, value, NULL);
    }

  gint cursor_size = g_settings_get_int (settings, "cursor-size");
  if (cursor_size > 0)
    g_object_set (gtk_settings, "gtk-cursor-theme-size", cursor_size, NULL);

  adw_style_manager_set_color_scheme (adw_style_manager_get_default (),
                                      ADW_COLOR_SCHEME_FORCE_DARK);
}
