#include <locale.h>

#include <gio/gio.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "unity-greeter-app.h"

gint
main (gint argc, gchar *argv[])
{
  (void) argc;

  g_log_set_writer_func (g_log_writer_journald, NULL, NULL);

  setlocale (LC_ALL, "");
  bindtextdomain (GETTEXT_PACKAGE, UNITY_GREETER_LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  g_set_application_name (_("Unity Greeter"));

  g_autoptr (UnityGreeterApp) app = unity_greeter_app_new ();

  gchar *forwarded[] = { argv[0], NULL };
  return g_application_run (G_APPLICATION (app), 1, forwarded);
}
