/* unity-greeter-user-setup-page.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <adwaita.h>

#include "unity-greeter-user.h"

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_USER_SETUP_PAGE (unity_greeter_user_setup_page_get_type ())

/* Page shown when an account still needs a first password. Asks for one,
   checks the strength, hands the result to AccountsService, then moves on
   to the login page. */
G_DECLARE_FINAL_TYPE (UnityGreeterUserSetupPage, unity_greeter_user_setup_page,
                      UNITY_GREETER, USER_SETUP_PAGE, AdwNavigationPage)

AdwNavigationPage *unity_greeter_user_setup_page_new (ActUser    *user,
                                                      GListModel *sessions);

G_END_DECLS
