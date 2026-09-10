/* unity-greeter-idle.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <glib.h>

G_BEGIN_DECLS

/* Asks logind to suspend after 3 minutes of seat inactivity. */
void unity_greeter_idle_watch (void);

G_END_DECLS
