/* unity-greeter-scale.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "unity-greeter-scale.h"

#include <string.h>

#include <glib/gstdio.h>

#define GREETER_BASE_CONFIG "/etc/unity-greeter/wayfire.ini"
#define OUTPUT_GROUP_PREFIX "output:"
#define SCALE_MAX           4.0
#define OUTPUTS_BASENAME    "outputs.ini"

static gboolean
connector_is_sane (const gchar *connector)
{
  if (*connector == '\0')
    return FALSE;

  for (const gchar *p = connector; *p != '\0'; p++)
    if (!g_ascii_isalnum (*p) && *p != '-' && *p != '_')
      return FALSE;

  return TRUE;
}

static void
append_output_scales (GString *composed, const gchar *path)
{
  g_autoptr (GKeyFile) keys = g_key_file_new ();
  if (!g_key_file_load_from_file (keys, path, G_KEY_FILE_NONE, NULL))
    return;

  g_auto (GStrv) groups = g_key_file_get_groups (keys, NULL);
  for (gsize i = 0; groups != NULL && groups[i] != NULL; i++)
    {
      if (!g_str_has_prefix (groups[i], OUTPUT_GROUP_PREFIX))
        continue;

      const gchar *connector = groups[i] + strlen (OUTPUT_GROUP_PREFIX);
      if (!connector_is_sane (connector))
        continue;

      g_autoptr (GError) error = NULL;
      gdouble scale = g_key_file_get_double (keys, groups[i], "scale", &error);
      if (error != NULL || scale <= 0.0 || scale > SCALE_MAX)
        continue;

      g_string_append_printf (composed, "\n[%s%s]\nscale = %.6f\n",
                              OUTPUT_GROUP_PREFIX, connector, scale);
    }
}

static gchar *
newest_outputs_ini (void)
{
  g_autoptr (GDir) dir = g_dir_open (UNITY_GREETER_MIRROR_ROOT, 0, NULL);
  if (dir == NULL)
    return NULL;

  gchar       *best = NULL;
  gint64       best_mtime = 0;
  const gchar *name;

  while ((name = g_dir_read_name (dir)) != NULL)
    {
      g_autofree gchar *ini =
        g_build_filename (UNITY_GREETER_MIRROR_ROOT, name, OUTPUTS_BASENAME, NULL);
      GStatBuf st;
      if (g_stat (ini, &st) != 0)
        continue;
      if (best == NULL || st.st_mtime > best_mtime)
        {
          g_free (best);
          best = g_steal_pointer (&ini);
          best_mtime = st.st_mtime;
        }
    }
  return best;
}

gboolean
unity_greeter_scale_other_user_published (void)
{
  g_autoptr (GDir) dir = g_dir_open (UNITY_GREETER_MIRROR_ROOT, 0, NULL);
  if (dir == NULL)
    return FALSE;

  const gchar *me = g_get_user_name ();
  const gchar *name;

  while ((name = g_dir_read_name (dir)) != NULL)
    {
      if (g_strcmp0 (name, me) == 0)
        continue;

      g_autofree gchar *path =
        g_build_filename (UNITY_GREETER_MIRROR_ROOT, name, OUTPUTS_BASENAME, NULL);
      if (g_file_test (path, G_FILE_TEST_EXISTS))
        return TRUE;
    }
  return FALSE;
}

gchar *
unity_greeter_scale_compose (void)
{
  g_autofree gchar *base = NULL;
  gsize            base_len = 0;
  if (!g_file_get_contents (GREETER_BASE_CONFIG, &base, &base_len, NULL))
    return NULL;

  g_autoptr (GString) composed = g_string_new_len (base, base_len);

  g_autofree gchar *outputs_path = newest_outputs_ini ();
  if (outputs_path != NULL)
    {
      if (composed->len > 0 && composed->str[composed->len - 1] != '\n')
        g_string_append_c (composed, '\n');
      append_output_scales (composed, outputs_path);
    }

  g_autofree gchar *out_path =
    g_build_filename (UNITY_GREETER_MIRROR_ROOT, "wayfire.ini", NULL);
  if (!g_file_set_contents (out_path, composed->str, composed->len, NULL))
    return NULL;
  return g_steal_pointer (&out_path);
}
