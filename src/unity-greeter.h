/* unity-greeter.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <gio/gio.h>
#include <unity-window.h>

G_BEGIN_DECLS

#define UNITY_TYPE_GREETER (unity_greeter_get_type ())

/* Greeter layer-shell surface, one instance per monitor. */
G_DECLARE_FINAL_TYPE (UnityGreeter, unity_greeter,
                      UNITY, GREETER, UnityWindow)

/* @users holds ActUser, @sessions holds UnityGreeterSession. */
UnityGreeter *unity_greeter_new (GtkApplication *app,
                                 GListModel     *users,
                                 GListModel     *sessions);

G_END_DECLS
