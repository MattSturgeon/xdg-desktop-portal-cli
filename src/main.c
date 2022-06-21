/* main.c
 *
 * Copyright 2022 Matt Sturgeon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "command.h"

#include <glib.h>
#include <stdlib.h>

// Caller is responsible for freeing returned array
gchar** split(gint start, gint length, gchar *array[])
{
    gsize size = length * sizeof(gchar*);
    gchar **copy = malloc(size);
    if (copy == NULL) {
      /* handle error */
    }

    memcpy(copy, array + start, size);
    return copy;
}

gint main (gint   argc, gchar *argv[])
{
    if (argc < 2)
    {
        g_printerr("Not enough arguments\n");
        return EXIT_FAILURE;
    }

    // Looks like argv[1] is portal, copy the rest of the args to give to the handler
    gsize size = (argc - 2) * sizeof(gchar*);
    gchar **copy = malloc(size);
    if (copy == NULL) {
        g_printerr ("Error copying arguments into smaller array\n");
        return EXIT_FAILURE;
    }
    memcpy(copy, argv + 2, size);

    Portal *portal = get_portal_from_string(argv[1]);
    if (portal == NULL)
    {
        g_printerr ("Unrecognised command \"%s\"\n", argv[1]);
        return EXIT_FAILURE;
    }
    Command *command = call_portal_handler(portal, argc - 2, copy);
    /*

  g_autoptr(GOptionContext) context = NULL;
  g_autoptr(GError) error = NULL;

  gboolean version = FALSE;
  GOptionEntry main_entries[] = {
    { "version", 0, 0, G_OPTION_ARG_NONE, &version, "Show program version", NULL },
    { NULL }
  };

  context = g_option_context_new ("- my command line tool");
  g_option_context_add_main_entries (context, main_entries, NULL);

  if (!g_option_context_parse (context, &argc, &argv, &error))
    {
      g_printerr ("%s\n", error->message);
      return EXIT_FAILURE;
    }

  if (version)
    {
      g_printerr ("%s\n", PACKAGE_VERSION);
      return EXIT_SUCCESS;
    }

     */

  return EXIT_SUCCESS;
}
