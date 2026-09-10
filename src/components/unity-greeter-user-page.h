/* unity-greeter-user-page.h
 *
 * Copyright 2026 Muqtadir
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
