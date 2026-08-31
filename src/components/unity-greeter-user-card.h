/* unity-greeter-user-card.h
 *
 * Copyright 2026 Muqtadir
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <adwaita.h>
#include <gtk/gtk.h>

#include "unity-greeter-user.h"

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_USER_CARD (unity_greeter_user_card_get_type ())

/* One user entry on the greeter screen. Emits "activated" when clicked. */
G_DECLARE_FINAL_TYPE (UnityGreeterUserCard, unity_greeter_user_card,
                      UNITY_GREETER, USER_CARD, GtkBox)

GtkWidget *unity_greeter_user_card_new (ActUser *user);

ActUser *unity_greeter_user_card_get_user (UnityGreeterUserCard *self);

G_END_DECLS
