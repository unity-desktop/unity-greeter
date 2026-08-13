#pragma once

#include <astal-4.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/**
 * UNITY_TYPE_GREETER:
 *
 * The #GType for #UnityGreeter.
 */
#define UNITY_TYPE_GREETER (unity_greeter_get_type ())

/**
 * UnityGreeter:
 *
 * The greeter shell surface. A layer-shell overlay covering the whole
 * display and grabbing exclusive keyboard input.
 */
G_DECLARE_FINAL_TYPE (UnityGreeter, unity_greeter,
                      UNITY, GREETER, AstalWindow)

/**
 * unity_greeter_new:
 * @app: the owning application.
 * @users: list model containing #UnityGreeterUser objects.
 * @sessions: list model containing #UnityGreeterSession objects.
 *
 * Creates the greeter shell surface.
 *
 * Returns: (transfer full): a new #UnityGreeter instance.
 */
UnityGreeter *unity_greeter_new (GtkApplication *app,
                                 GListModel     *users,
                                 GListModel     *sessions);

G_END_DECLS
