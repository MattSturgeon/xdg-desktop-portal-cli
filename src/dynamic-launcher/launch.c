#include "dynamic-launcher.h"

gint launch_command_handler(gint argc, gchar* argv[]);

Command launch_command = {
    .name      = "launch",
    .handler   = &launch_command_handler,
    .usage     = "<desktop_file_name>",
    .help_text = "Runs the given desktop file.",
    .options =
        {
            {NULL},
        },
};

gint launch_command_handler(gint argc, gchar* argv[])
{
  g_printerr("launch command not yet implemented!\n");
  return EXIT_FAILURE;
}
