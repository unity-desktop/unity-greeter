/* unity-greeter-strength.c
 *
 * Copyright 2026 Muqtadir
 *
 * The pwquality hint map and scoring bands are adapted from
 * gnome-control-center / gnome-initial-setup's pw-utils.c
 * (Copyright 2012 Red Hat, Inc, written by Matthias Clasen),
 * originally released under the GNU GPL v2 or later.
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

#include "unity-greeter-strength.h"

#include <string.h>

#include <glib/gi18n.h>
#include <pwquality.h>

struct _UnityGreeterStrength
{
  GObject               parent_instance;
  pwquality_settings_t *settings;
};

G_DEFINE_FINAL_TYPE (UnityGreeterStrength, unity_greeter_strength, G_TYPE_OBJECT)

static const gchar *
hint_for_pwq_error (gint error)
{
  switch (error)
    {
    case PWQ_ERROR_SAME_PASSWORD:
      return C_("Password hint", "The new password needs to be different from the old one");
    case PWQ_ERROR_CASE_CHANGES_ONLY:
      return C_("Password hint", "Try changing some letters and numbers");
    case PWQ_ERROR_TOO_SIMILAR:
    case PWQ_ERROR_ROTATED:
      return C_("Password hint", "Try changing the password a bit more");
    case PWQ_ERROR_USER_CHECK:
      return C_("Password hint", "A password without your user name would be stronger");
    case PWQ_ERROR_GECOS_CHECK:
      return C_("Password hint", "Try to avoid using your name in the password");
    case PWQ_ERROR_BAD_WORDS:
      return C_("Password hint", "Try to avoid some of the words included in the password");
    case PWQ_ERROR_CRACKLIB_CHECK:
      return C_("Password hint", "Avoid common words");
    case PWQ_ERROR_PALINDROME:
      return C_("Password hint", "Try to avoid reordering existing words");
    case PWQ_ERROR_MIN_DIGITS:
      return C_("Password hint", "Try to use more numbers");
    case PWQ_ERROR_MIN_UPPERS:
      return C_("Password hint", "Try to use more uppercase letters");
    case PWQ_ERROR_MIN_LOWERS:
      return C_("Password hint", "Try to use more lowercase letters");
    case PWQ_ERROR_MIN_OTHERS:
      return C_("Password hint", "Try to use more special characters, like punctuation");
    case PWQ_ERROR_MIN_CLASSES:
      return C_("Password hint", "Try to use a mixture of letters, numbers and punctuation");
    case PWQ_ERROR_MAX_CONSECUTIVE:
      return C_("Password hint", "Try to avoid repeating the same character");
    case PWQ_ERROR_MAX_CLASS_REPEAT:
      return C_("Password hint", "Try to avoid repeating the same type of character");
    case PWQ_ERROR_MAX_SEQUENCE:
      return C_("Password hint", "Try to avoid sequences like 1234 or abcd");
    case PWQ_ERROR_MIN_LENGTH:
      return C_("Password hint", "Password needs to be longer");
    case PWQ_ERROR_EMPTY_PASSWORD:
      return C_("Password hint", "Mix uppercase and lowercase and try to use a number or two");
    default:
      return C_("Password hint", "Try adding more letters, numbers and punctuation");
    }
}

static gint
pw_min_length (pwquality_settings_t *settings)
{
  gint value = 0;
  if (settings != NULL)
    pwquality_get_int_value (settings, PWQ_SETTING_MIN_LENGTH, &value);
  return value;
}

static void
unity_greeter_strength_finalize (GObject *object)
{
  UnityGreeterStrength *self = UNITY_GREETER_STRENGTH (object);
  g_clear_pointer (&self->settings, pwquality_free_settings);
  G_OBJECT_CLASS (unity_greeter_strength_parent_class)->finalize (object);
}

static void
unity_greeter_strength_class_init (UnityGreeterStrengthClass *klass)
{
  G_OBJECT_CLASS (klass)->finalize = unity_greeter_strength_finalize;
}

static void
unity_greeter_strength_init (UnityGreeterStrength *self)
{
  self->settings = pwquality_default_settings ();
  if (self->settings != NULL)
    pwquality_read_config (self->settings, NULL, NULL);
}

UnityGreeterStrength *
unity_greeter_strength_new (void)
{
  return g_object_new (UNITY_GREETER_TYPE_STRENGTH, NULL);
}

void
unity_greeter_strength_check (UnityGreeterStrength *self,
                              const gchar          *password,
                              const gchar          *username,
                              const gchar         **out_hint,
                              gint                 *out_level,
                              const gchar         **out_level_hint)
{
  g_return_if_fail (UNITY_GREETER_IS_STRENGTH (self));

  gint length = password != NULL ? (gint) strlen (password) : 0;
  gint rv = -1;
  void *aux = NULL;

  if (self->settings != NULL && length > 0)
    rv = pwquality_check (self->settings, password, NULL, username, &aux);

  gdouble strength = CLAMP (0.01 * rv, 0.0, 1.0);
  gint         level;
  const gchar *level_hint;

  if (rv < 0)
    {
      level_hint = C_("Password strength", "poor");
      level = length > 0 ? 1 : 0;
    }
  else if (strength < 0.50)
    {
      level_hint = C_("Password strength", "weak");
      level = 2;
    }
  else if (strength < 0.75)
    {
      level_hint = C_("Password strength", "strong");
      level = 3;
    }
  else if (strength < 0.90)
    {
      level_hint = C_("Password strength", "strong");
      level = 4;
    }
  else
    {
      level_hint = C_("Password strength", "excellent");
      level = 5;
    }

  if (out_hint != NULL)
    {
      if (length > 0 && length < pw_min_length (self->settings))
        *out_hint = hint_for_pwq_error (PWQ_ERROR_MIN_LENGTH);
      else
        *out_hint = hint_for_pwq_error (rv);
    }
  if (out_level != NULL)
    *out_level = level;
  if (out_level_hint != NULL)
    *out_level_hint = level_hint;
}
