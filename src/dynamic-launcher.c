#include "dynamic-launcher.h"
#include "command.h"

gint *dynamic_launcher_install(gint argc, gchar *argv[], gboolean show_help)
{
    g_print("Running dynamic launcher install with %d args\n", argc);

    for (int i = 0; i < argc; i++)
    {
        g_print("    [%d]: %s\n", i, argv[i]);
    }

    if (show_help) {
        g_print ("Handling --help from withing the Command handler...\n");

    }



    return EXIT_SUCCESS;
}
