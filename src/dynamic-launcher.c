#include "dynamic-launcher.h"

Command *dynamic_launcher_install(gint argc, gchar *argv[])
{
    g_print("Running dynamic launcher install with %d args\n", argc);

    for (int i = 0; i < argc; i++)
    {
        g_print("    [%d]: %s\n", i, argv[i]);
    }

    return EXIT_SUCCESS;
}
