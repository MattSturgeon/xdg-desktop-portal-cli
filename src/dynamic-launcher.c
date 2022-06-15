#define GETTEXT_PACKAGE "gtk20"

#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>

#define HELP_SUMMARY \
    "The DynamicLauncher portal allows sandboxed (or unsandboxed) applications to install launchers (.desktop files) which have an icon associated with them\n" \
    "and which execute a command in the application. The desktop environment would display the launcher to the user in its menu of installed applications. For\n" \
    "example this can be used by a sandboxed browser to install web app launchers. The portal also allows apps to uninstall the launchers, launch them, and read\n" \
    "the desktop file and icon data for them.\n" \
    "\n" \
    "Available Commands:\n" \
    "  install                  Installs a .desktop launcher and icon into appropriate directories to allow the desktop environment to find them. Please note that this\n" \
    "                           method overwrites any existing launcher with the same id. \n" \
    "                           Presents a dialog to the user to allow them to see the icon, potentially change the name, and confirm installation of the launcher. \n" \
    "\n" \
    "                           If --name-editable is set, then the user will be able to edit the desktop entry's name.\n" \
    "                           If --icon-editable is set, then the user will be able to edit the desktop entry's icon.\n" \
    "                           Requires: --id, --name, --icon-file, --content\n" \
    "  uninstall                Removes the desktop file and corresponding icon from the appropriate directories to remove the launcher referred to by --id.\n" \
    "                           Requires: --id\n" \
    "  get                      Prints the contents of a desktop file that matches --id\n" \
    "                           Requires: --id\n" \
    "  geticon                  Get the icon used by the a desktop file that matches --id. The icon is written to --icon-file. Any metadata (such as its size) is printed to stdout.\n" \
    "                           Requires: --id, --icon-file\n" \
    "  launch                   Run a desktop file that matches --id.\n" \
    "                           This method only works for desktop files that were created by the dynamic launcher portal. \n" \
    "                           Requires: --id"

// Main options
static gboolean version = FALSE;
static gboolean verbose = FALSE;
static gboolean icon_editable = FALSE;
static gboolean name_editable = FALSE;
static gchar* icon_filename = "";
static gchar* entry_id = "";
static gchar* entry_name = "";
static gchar* entry = "";

static GOptionEntry entries[] =
    {
        {"verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL},
        {"version", 0, 0, G_OPTION_ARG_NONE, &version, "Show version information", NULL},
        {"icon-editable", 0, 0, G_OPTION_ARG_NONE, &icon_editable, "Whether or not the user will be able edit the icon. Used when installing a new desktop entry.", NULL},
        {"name-editable", 0, 0, G_OPTION_ARG_NONE, &name_editable, "Whether or not the user will be able edit the name of the desktop entry. Used when installing a new desktop entry.", NULL},
        {"icon-file", 'i', 0, G_OPTION_ARG_FILENAME, &icon_filename, "Path to an app icon. Either the icon to use when installing, or the path to save the icon when running geticon.", "FILE"},
        {"id", 0, 0, G_OPTION_ARG_STRING, &entry_id, "The desktop_file_id of the dekstop entry. This ID is used as the filename for the desktop entry.\n"
            "                           The desktop_file_id must have \".desktop\" as a suffix. Except in the special case when the calling process has no associated app ID,\n"
            "                           desktop_file_id must have the app ID followed by a period as a prefix. " , "ID"},
        {"name", 0, 0, G_OPTION_ARG_STRING, &entry_name, "The name of the dekstop entry. Used when installing a new desktop entry.", "NAME"},
        {"content", 0, 0, G_OPTION_ARG_STRING, &entry, "The actual content of the desktop entry. Used when installing a new desktop entry. Some parts may be replaced or edited by the portal,\n"
            "                           in particular Icon, Name and Exec.", "CONTENT"},
        {NULL}
    };



GVariant* load_icon(char* filename, GError *error) {
    GError *internal_error = NULL;
    GFile *file = g_file_new_for_path(filename);
    
    // TODO check file is valid
    GFileInfo *info = g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_SIZE, 0, NULL, &internal_error);
    
    // Check for errors:
    if (info == NULL) {
        g_printerr("Looks like file info query failed... Error: %s\n", internal_error->message);
        error=internal_error;
        return NULL;
    } else if (!g_file_info_has_attribute(info, G_FILE_ATTRIBUTE_STANDARD_SIZE)) {
        g_printerr("Hmmm, something went wrong. Icon file has no size attribute!\n");
        error = internal_error;
        return NULL;
    }
    
    g_print("Icon file size is %s\n", g_file_info_get_attribute_as_string(info, G_FILE_ATTRIBUTE_STANDARD_SIZE));
    
    // Serialize the icon
    GIcon *icon = g_file_icon_new(file);
    if (icon == NULL) {
        g_printerr("Error loading file as icon...\n");
        // TODO set error
        return NULL;
    }
    GVariant* serialized_icon = g_icon_serialize(icon);

    g_object_unref(icon);
    g_object_unref(info);
    g_object_unref(file);

    return serialized_icon;
}

int install(gchar* desktop_entry_id, gchar* name, gchar* icon_filename, gchar* desktop_entry_content, gboolean name_editable, gboolean icon_editable) {
    g_printerr("Error: install command is not implemented yet!\n");
    return 1;
}

int uninstall(gchar* desktop_entry_id) {
    g_printerr("Error: uninstall command is not implemented yet!\n");
    return 1;
}

int get_desktop_entry(gchar* desktop_entry_id) {
    g_printerr("Error: get command is not implemented yet!\n");
    return 1;
}

int get_icon_from_desktop_entry(gchar* desktop_entry_id, gchar* icon_filename) {
    g_printerr("Error: geticon command is not implemented yet!\n");
    return 1;
}

int launch_desktop_entry(gchar* desktop_entry_id) {
    g_printerr("Error: launch command is not implemented yet!\n");
    return 1;
}

int main(int argc, char *argv[])
{
    GError *error = NULL;

    // First of all, parse our options and setup --help
    GOptionContext *context = g_option_context_new("<command> ");
    g_option_context_set_summary(context, HELP_SUMMARY);
    g_option_context_set_description(context, "This is the description");
    g_option_context_add_main_entries(context, entries, GETTEXT_PACKAGE);

    if (!g_option_context_parse(context, &argc, &argv, &error))
    {
        g_printerr("Error: %s\n", error->message);
        exit(1);
    }
    g_option_context_free (context);

	if (version) {
		g_print ("\n" "TODO: print version information and license" "\n");
		exit(0);
	}

    if (argc < 2) {
        g_printerr("Error: Missing <command> argument\n");
        return 1;
    }

    // Run the relevent command, passing in relevent options
    gchar* command = g_ascii_strdown(g_strstrip(argv[1]), -1);
    if (strcmp(command, "install")==0) {
        return install(entry_id, entry_name, icon_filename, entry, name_editable, icon_editable);
    } else if (strcmp(command, "uninstall")==0) {
        return uninstall(entry_id);
    } else if (strcmp(command, "get")==0) {
        return get_desktop_entry(entry_id);
    } else if (strcmp(command, "geticon")==0) {
        return get_icon_from_desktop_entry(entry_id, icon_filename);
    } else if (strcmp(command, "launch")==0) {
        return launch_desktop_entry(entry_id);
    } else {
        g_printerr("Error: Unrecognised command \"%s\"\n", argv[1]);
        return 1;
    }

    return 0;
}