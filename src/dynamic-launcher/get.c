#include "dynamic-launcher.h"

gint get_command_handler(gint argc, gchar* argv[]);

Command get_command = {
    .name      = "get",
    .handler   = &get_command_handler,
    .usage     = "<desktop_file_name>",
    .help_text = "Prints the content of the given desktop file.",
    .options =
        {
            {NULL},
        },
};

gint get_command_handler(gint argc, gchar* argv[])
{
  g_printerr("get command not yet implemented!\n");
  return EXIT_FAILURE;
}
