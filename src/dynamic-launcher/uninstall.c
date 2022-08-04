#include "dynamic-launcher.h"

gint uninstall_command_handler(gint argc, gchar* argv[]);

Command uninstall_command = {
    .name    = "uninstall",
    .handler = uninstall_command_handler,
    .usage   = "<desktop_file_name>",
    .help_text =
        "Removes the desktop file and corresponding icon from the appropriate "
        "directories to remove the launcher referred to by.",
    .options =
        {
            {NULL},
        },
};

gint uninstall_command_handler(gint argc, gchar* argv[])
{
  g_printerr("uninstall command not yet implemented!\n");
  return EXIT_FAILURE;
}
