#pragma once

#include <adwaita.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

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
 * Main greeter window that presents users and available sessions.
 */
G_DECLARE_FINAL_TYPE (UnityGreeter, unity_greeter,
                      UNITY, GREETER, AdwApplicationWindow)

/**
 * unity_greeter_new:
 * @app: the owning application.
 * @users: list model containing #UnityGreeterUser objects.
 * @sessions: list model containing #UnityGreeterSession objects.
 *
 * Creates the greeter window.
 *
 * Returns: (transfer full): a new #UnityGreeter instance.
 */
UnityGreeter *unity_greeter_new (GtkApplication *app,
                                 GListModel     *users,
                                 GListModel     *sessions);

G_END_DECLS
