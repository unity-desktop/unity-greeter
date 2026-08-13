#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <glib/gstdio.h>

#include "unity-greeter-defs.h"

static gchar *
newest_phoc_config (void)
{
  g_autoptr (GDir) dir = g_dir_open (UNITY_GREETER_MIRROR_ROOT, 0, NULL);
  if (dir == NULL)
    return NULL;

  gchar       *best = NULL;
  gint64       best_mtime = 0;
  const gchar *name;

  while ((name = g_dir_read_name (dir)) != NULL)
    {
      g_autofree gchar *ini =
        g_build_filename (UNITY_GREETER_MIRROR_ROOT, name, "phoc.ini", NULL);
      GStatBuf st;
      if (g_stat (ini, &st) != 0)
        continue;
      if (best == NULL || st.st_mtime > best_mtime)
        {
          g_free (best);
          best = g_steal_pointer (&ini);
          best_mtime = st.st_mtime;
        }
    }
  return best;
}

gint
main (gint argc, gchar *argv[])
{
  (void) argc;
  (void) argv;

  g_setenv ("GDK_BACKEND",   "wayland",      TRUE);
  g_setenv ("DCONF_PROFILE", "greetd",       TRUE);
  g_setenv ("WLR_BACKENDS",  "drm,libinput", TRUE);

  g_autofree gchar *config = newest_phoc_config ();

  if (config != NULL)
    execlp ("phoc", "phoc", "-C", config,
            "--no-xwayland", "-E", "unity-greeter", (gchar *) NULL);
  else
    execlp ("phoc", "phoc",
            "--no-xwayland", "-E", "unity-greeter", (gchar *) NULL);

  g_printerr ("unity-greeter-session: exec phoc: %s\n", g_strerror (errno));
  return EXIT_FAILURE;
}
