/* unity-greeter-user-page.h
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

#include "unity-greeter-user.h"

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_USER_PAGE (unity_greeter_user_page_get_type ())

/* Navigation page that authenticates the selected user. */
G_DECLARE_FINAL_TYPE (UnityGreeterUserPage, unity_greeter_user_page,
                      UNITY_GREETER, USER_PAGE, AdwNavigationPage)

AdwNavigationPage *unity_greeter_user_page_new (ActUser    *user,
                                                GListModel *sessions);

G_END_DECLS
