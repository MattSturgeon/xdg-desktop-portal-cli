#include <stdlib.h>

#include "dynamic-launcher.h"

gint get_icon_command_handler(gint argc, gchar* argv[]);

Command get_icon_command = {
    .name    = "get_icon",
    .handler = &get_icon_command_handler,
    .usage   = "<desktop_file_name> [output_file]",
    .help_text =
        "Get the icon used by the given desktop file and save a copy to the "
        "given output_file.",
    .options =
        {
            // TODO support base64 encoding options
            // TODO support printing to stdout
            {NULL},
        },
};

gint get_icon_command_handler(gint argc, gchar* argv[])
{
  g_printerr("get_icon command not yet implemented!\n");
  return EXIT_FAILURE;
}
