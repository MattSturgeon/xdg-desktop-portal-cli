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

#include <glib.h>
#include <stdlib.h>

#include "command.h"
#include "event-loop.h"
#include "xdg-portal.h"

// Caller is responsible for freeing returned array
gchar** split(gint start, gint length, gchar* array[])
{
  gsize   size = length * sizeof(gchar*);
  gchar** copy = malloc(size);
  if (copy == NULL) {
    /* handle error */
  }

  memcpy(copy, array + start, size);
  return copy;
}

gint main(gint argc, gchar* argv[])
{
  // Use GOptionContext to parse --option flags.
  // We only care about --help and --version at this point,
  // we'll let Portals and Commands parse again later.
  g_autoptr(GOptionContext) context = NULL;
  g_autoptr(GError) error           = NULL;

  gboolean     help           = FALSE;
  gboolean     version        = FALSE;
  GOptionEntry main_entries[] = {
      {"help", 'h', 0, G_OPTION_ARG_NONE, &help, NULL, NULL},
      {"help", '?', 0, G_OPTION_ARG_NONE, &help, NULL, NULL},
      {"version", 0, 0, G_OPTION_ARG_NONE, &version, NULL, NULL},
      {NULL},
  };

  context = g_option_context_new("");
  // We want to generate our own help page
  g_option_context_set_help_enabled(context, FALSE);
  // Ignore unknown options at this stage so that they can be parsed later
  g_option_context_set_ignore_unknown_options(context, TRUE);
  g_option_context_add_main_entries(context, main_entries, NULL);
  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    g_printerr("%s\n", error->message);
    return EXIT_FAILURE;
  }

  if (version) {
    g_print("%s\n", PACKAGE_VERSION);
    return EXIT_SUCCESS;
  }

  if (argc < 2) {
    if (help) {
      gchar* help_str = get_help();
      g_print("%s\n", help_str);
      g_free(help_str);
      return EXIT_SUCCESS;
    }

    g_printerr("You didn't specify a portal!\n");
    return EXIT_FAILURE;
  }

  // Looks like argv[1] is portal, copy the rest of the args to give to the
  // handler
  gsize   copy_len = argc - 2;
  gsize   size     = copy_len * sizeof(gchar*);
  gchar** copy     = malloc(size);
  if (copy == NULL) {
    g_printerr("Error assigning memory to copy arguments into smaller array\n");
    return EXIT_FAILURE;
  }
  memcpy(copy, argv + 2, size);

  // TODO Consider extracting this point onwards into command.c (so we can use
  // Portal/Command struct members without creating accessor methods)
  //  Look for a portal that matches args
  gchar*  help_text = NULL;
  Portal* portal    = get_portal_from_string(argv[1]);
  if (portal == NULL) {
    g_printerr("Unrecognised portal \"%s\"\n", argv[1]);
    goto failure;
  }

  if (copy_len < 1) {
    if (help) {
      // Show help for portal
      help_text = get_help_from_portal(portal);
      g_print("%s", help_text);
      g_free(help_text);
      goto success;
    }
    g_printerr("You didn't specify a command!\n");
    goto failure;
  }

  // Look for a command in portal that matches args
  Command* command = get_command_from_string(portal, copy[0]);
  if (command == NULL) {
    g_printerr("Unrecognised command \"%s\"\n", argv[2]);
    goto failure;
  }
  if (help) {
    help_text = get_help_from_command(command);
    g_print("%s", help_text);
    g_free(help_text);
    goto success;
  }

  // Now we have a command, parse its options
  g_option_context_set_ignore_unknown_options(context, FALSE);
  g_option_context_add_main_entries(context, command->options, NULL);
  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    g_printerr("%s\n", error->message);
    return EXIT_FAILURE;
  }

  // Get an instance of XdgPortal
  init_xdg_portal();

  // Finally, run the command
  gint status = call_command_handler(command, copy_len - 1, &copy[1]);

  // create an event loop if needed
  start_loop();

  free(copy);
  return status;

success:
  free(copy);
  return EXIT_SUCCESS;
failure:
  free(copy);
  return EXIT_FAILURE;
}
