#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_APP (unity_greeter_app_get_type ())

G_DECLARE_FINAL_TYPE (UnityGreeterApp, unity_greeter_app,
                      UNITY_GREETER, APP, AdwApplication)

UnityGreeterApp *unity_greeter_app_new (gboolean demo);

G_END_DECLS
