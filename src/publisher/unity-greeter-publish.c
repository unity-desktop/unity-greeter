/* unity-greeter-publish.c
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "unity-greeter-publish.h"

#include <glib/gstdio.h>

gchar *
unity_greeter_publish_path (const gchar *user_name,
                            const gchar *basename)
{
  return g_build_filename (UNITY_GREETER_MIRROR_ROOT, user_name, basename, NULL);
}

gboolean
unity_greeter_publish_bytes (const gchar *basename,
                             GBytes      *contents,
                             GError     **error)
{
  g_autofree gchar *path = unity_greeter_publish_path (g_get_user_name (), basename);
  g_autofree gchar *dir = g_path_get_dirname (path);
  g_mkdir_with_parents (dir, 0755);

  gsize size;
  const gchar *data = g_bytes_get_data (contents, &size);

  return g_file_set_contents_full (path, data, size,
                                   G_FILE_SET_CONTENTS_CONSISTENT, 0644, error);
}
