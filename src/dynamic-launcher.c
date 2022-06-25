#include "dynamic-launcher.h"
#include "command.h"

static gboolean icon_editable = FALSE;
static gboolean name_editable = FALSE;
static gchar *icon_filename = NULL;
static gchar *entry_name = NULL;
static gchar *entry_content = NULL;


static Command install_command = {
    .name = "install",
    .usage = "<desktop_file_name>",
    .help_text = "TODO: copy from docs... installs a desktop entry onto the host system...",
    .handler = &dynamic_launcher_install,
    .options = {
        // TODO
        {"icon-editable", 0, 0, G_OPTION_ARG_NONE, &icon_editable, "Whether or not the user will be able edit the icon. Used when installing a new desktop entry.", NULL},
        {"name-editable", 0, 0, G_OPTION_ARG_NONE, &name_editable, "Whether or not the user will be able edit the name of the desktop entry. Used when installing a new desktop entry.", NULL},
        {"icon-file", 'i', 0, G_OPTION_ARG_FILENAME, &icon_filename, "Path to an app icon. Either the icon to use when installing, or the path to save the icon when running geticon.", "FILE"},
        {"name", 0, 0, G_OPTION_ARG_STRING, &entry_name, "The name of the dekstop entry. Used when installing a new desktop entry.", "NAME"},
        {"content", 0, 0, G_OPTION_ARG_STRING, &entry_content, "The actual content of the desktop entry. Used when installing a new desktop entry. Some parts may be replaced or edited by the portal,\n"
            "                           in particular Icon, Name and Exec.", "CONTENT"},
        { NULL },
    }
};

static Command uninstall_command = {
    .name = "uninstall",
    .help_text = "TODO: copy from docs... removes a desktop entry from the host system...",
    .options = {
        // TODO
        { NULL },
    }
};

static Command get_command = {
    .name = "get",
    .help_text = "TODO: copy from docs... gets a desktop entry from the host system...",
    .options = {
        // TODO
        { NULL },
    }
};

static Command get_icon_command = {
    .name = "get_icon",
    .help_text = "TODO: copy from docs... gets the icon from a desktop entry on the host system...",
    .options = {
        // TODO
        { NULL },
    }
};

static Command launch_command = {
    .name = "launch",
    .help_text = "TODO: copy from docs... runs a desktop entry found on the host system...",
    .options = {
        // TODO
        { NULL },
    }
};

Portal dynamic_launcher_portal = {
    .name = "dynamic-launcher",
    .aliases = { "launcher", "desktop-entry" },
    .help_text = "TODO: Copy paste from docs... describe what the dynamic-launcher portal does...",
    .commands = {
        &install_command,
        &uninstall_command,
        &get_command,
        &get_icon_command,
        &launch_command,
    }
};

gint dynamic_launcher_install(gint argc, gchar *argv[], gboolean show_help)
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
