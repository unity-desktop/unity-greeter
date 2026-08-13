#include "unity-greeter-app.h"

#include "unity-greeter-session-list.h"
#include "unity-greeter-user.h"
#include "unity-greeter.h"

#define INTERFACE_SCHEMA "org.gnome.desktop.interface"

struct _UnityGreeterApp
{
  AdwApplication parent_instance;

  GListModel *users;
  GListModel *sessions;
};

G_DEFINE_FINAL_TYPE (UnityGreeterApp, unity_greeter_app, ADW_TYPE_APPLICATION)

static void
apply_string_key (GSettings   *settings,
                  const gchar *gsettings_key,
                  const gchar *gtk_property)
{
  g_autofree gchar *value = g_settings_get_string (settings, gsettings_key);
  if (value == NULL || *value == '\0')
    return;

  GtkSettings *gtk_settings = gtk_settings_get_default ();
  if (gtk_settings != NULL)
    g_object_set (gtk_settings, gtk_property, value, NULL);
}

static void
apply_appearance (void)
{
  GSettingsSchemaSource *source = g_settings_schema_source_get_default ();
  g_autoptr (GSettingsSchema) schema =
    source != NULL
      ? g_settings_schema_source_lookup (source, INTERFACE_SCHEMA, TRUE)
      : NULL;

  if (schema == NULL)
    {
      g_warning ("%s not installed, skipping appearance", INTERFACE_SCHEMA);
      return;
    }

  g_autoptr (GSettings) settings = g_settings_new_full (schema, NULL, NULL);

  apply_string_key (settings, "font-name",    "gtk-font-name");
  apply_string_key (settings, "icon-theme",   "gtk-icon-theme-name");
  apply_string_key (settings, "cursor-theme", "gtk-cursor-theme-name");

  gint cursor_size = g_settings_get_int (settings, "cursor-size");
  if (cursor_size > 0)
    g_object_set (gtk_settings_get_default (),
                  "gtk-cursor-theme-size", cursor_size,
                  NULL);

  adw_style_manager_set_color_scheme (adw_style_manager_get_default (),
                                      ADW_COLOR_SCHEME_FORCE_DARK);
}

static void
unity_greeter_app_startup (GApplication *application)
{
  UnityGreeterApp *self = UNITY_GREETER_APP (application);

  G_APPLICATION_CLASS (unity_greeter_app_parent_class)->startup (application);
  apply_appearance ();

  self->sessions = unity_greeter_session_list_new ();
  self->users    = unity_greeter_users_new ();
}

static void
unity_greeter_app_activate (GApplication *application)
{
  UnityGreeterApp *self = UNITY_GREETER_APP (application);

  UnityGreeter *window = unity_greeter_new (GTK_APPLICATION (self),
                                            self->users, self->sessions);
  gtk_window_present (GTK_WINDOW (window));
}

static void
unity_greeter_app_dispose (GObject *object)
{
  UnityGreeterApp *self = UNITY_GREETER_APP (object);

  g_clear_object (&self->users);
  g_clear_object (&self->sessions);

  G_OBJECT_CLASS (unity_greeter_app_parent_class)->dispose (object);
}

static void
unity_greeter_app_class_init (UnityGreeterAppClass *klass)
{
  GObjectClass      *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *app_class    = G_APPLICATION_CLASS (klass);

  object_class->dispose = unity_greeter_app_dispose;
  app_class->startup    = unity_greeter_app_startup;
  app_class->activate   = unity_greeter_app_activate;
}

static void
unity_greeter_app_init (UnityGreeterApp *self)
{
  (void) self;
}

UnityGreeterApp *
unity_greeter_app_new (void)
{
  return g_object_new (UNITY_GREETER_TYPE_APP,
                       "application-id",     "org.unity.Greeter",
                       "resource-base-path", "/org/unity/Greeter",
                       NULL);
}
