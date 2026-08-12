#pragma once

#include <adwaita.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define UNITY_TYPE_GREETER (unity_greeter_get_type ())

G_DECLARE_FINAL_TYPE (UnityGreeter, unity_greeter,
                      UNITY, GREETER, AdwApplicationWindow)

UnityGreeter *unity_greeter_new (GtkApplication *app,
                                 GListModel     *users,
                                 GListModel     *sessions);

G_END_DECLS
