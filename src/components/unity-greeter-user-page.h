#pragma once

#include <adwaita.h>

#include "unity-greeter-user.h"

G_BEGIN_DECLS

/**
 * UNITY_GREETER_TYPE_USER_PAGE:
 *
 * The #GType for #UnityGreeterUserPage.
 */
#define UNITY_GREETER_TYPE_USER_PAGE (unity_greeter_user_page_get_type ())

/**
 * UnityGreeterUserPage:
 *
 * Navigation page for authenticating a selected user.
 */
G_DECLARE_FINAL_TYPE (UnityGreeterUserPage, unity_greeter_user_page,
                      UNITY_GREETER, USER_PAGE, AdwNavigationPage)

/**
 * unity_greeter_user_page_new:
 * @user: the selected user.
 * @sessions: list model containing #UnityGreeterSession objects.
 *
 * Creates a new user authentication page.
 *
 * Returns: (transfer full): a new #AdwNavigationPage.
 */
AdwNavigationPage *unity_greeter_user_page_new (UnityGreeterUser *user,
                                                GListModel       *sessions);

G_END_DECLS
