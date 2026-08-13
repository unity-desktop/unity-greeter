#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

/**
 * UNITY_GREETER_TYPE_APP:
 *
 * The #GType for #UnityGreeterApp.
 */
#define UNITY_GREETER_TYPE_APP (unity_greeter_app_get_type ())

/**
 * UnityGreeterApp:
 *
 * The top-level application object for Unity Greeter.
 */
G_DECLARE_FINAL_TYPE (UnityGreeterApp, unity_greeter_app,
                      UNITY_GREETER, APP, AdwApplication)

/**
 * unity_greeter_app_new:
 *
 * Creates a new #UnityGreeterApp instance.
 *
 * Returns: (transfer full): a new #UnityGreeterApp.
 */
UnityGreeterApp *unity_greeter_app_new (void);

G_END_DECLS
