/* unity-greeter-session.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>

#include "unity-greeter-scale.h"

gint
main (void)
{
  g_setenv ("GDK_BACKEND",   "wayland",      TRUE);
  g_setenv ("WLR_BACKENDS",  "drm,libinput", TRUE);

  g_autofree gchar *config = unity_greeter_scale_compose ();
  if (config == NULL)
    {
      g_printerr ("unity-greeter-session: cannot compose wayfire config\n");
      return EXIT_FAILURE;
    }

  execlp ("wayfire", "wayfire", "-c", config, (gchar *) NULL);

  g_printerr ("unity-greeter-session: exec wayfire: %s\n", g_strerror (errno));
  return EXIT_FAILURE;
}
