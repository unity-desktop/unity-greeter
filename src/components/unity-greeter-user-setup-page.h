/* unity-greeter-user-setup-page.h
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

/**
 * UNITY_GREETER_TYPE_USER_SETUP_PAGE:
 *
 * Type identifier for #UnityGreeterUserSetupPage.
 */
#define UNITY_GREETER_TYPE_USER_SETUP_PAGE (unity_greeter_user_setup_page_get_type ())

/**
 * UnityGreeterUserSetupPage:
 *
 * Page shown when an account still needs a first password. Asks for
 * one, checks the strength, and hands the result to AccountsService.
 * When the write is done we move on to the login page.
 */
G_DECLARE_FINAL_TYPE (UnityGreeterUserSetupPage, unity_greeter_user_setup_page,
                      UNITY_GREETER, USER_SETUP_PAGE, AdwNavigationPage)

/**
 * unity_greeter_user_setup_page_new:
 * @user: the account this page is for.
 * @sessions: sessions to pass on to the login page.
 *
 * Returns: (transfer full): a new #AdwNavigationPage.
 */
AdwNavigationPage *unity_greeter_user_setup_page_new (UnityGreeterUser *user,
                                                      GListModel       *sessions);

G_END_DECLS
