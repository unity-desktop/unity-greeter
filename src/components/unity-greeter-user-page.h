#pragma once

#include <adwaita.h>

#include "unity-greeter-user.h"

G_BEGIN_DECLS

#define UNITY_GREETER_TYPE_USER_PAGE (unity_greeter_user_page_get_type ())

G_DECLARE_FINAL_TYPE (UnityGreeterUserPage, unity_greeter_user_page,
                      UNITY_GREETER, USER_PAGE, AdwNavigationPage)

AdwNavigationPage *unity_greeter_user_page_new (UnityGreeterUser *user,
                                                GListModel       *sessions);

G_END_DECLS
