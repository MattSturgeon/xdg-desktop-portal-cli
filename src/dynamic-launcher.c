#define GETTEXT_PACKAGE "gtk20"

#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <libportal/portal.h>

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

static XdpPortal *portal = NULL;
static GMainLoop *loop = NULL;

// Datatype used to pass data from install() to install_callback()
typedef struct _InstallCallbackData{
    // TODO Consider what data needs to be passed to callback(s)
    gchar* desktop_entry_id;
    gchar* desktop_entry_content;
} InstallCallbackData;

// Loads the icon into memory and serializes it into a GVariant
GVariant* load_icon(char* filename, GError **error) {
    // Load the icon
    // TODO add some checks to make sure we don't load a ridiculously big icon into memory
    GError *internal_error = NULL;
    gchar *etag;
    GFile *file = g_file_new_for_path(filename);
    GBytes *bytes = g_file_load_bytes(file, NULL, &etag, &internal_error);
    if (internal_error != NULL) {
        g_printerr("Error loading file into memory: %s\n", internal_error->message);
        *error = internal_error;

        g_bytes_unref(bytes);
        g_object_unref(file);
        return NULL;
    }
    g_print("File loaded... etag: %s\n", etag);
    g_print("%d bytes read\n", g_bytes_get_size(bytes));
    
    // Serialize the icon
    GIcon *icon = g_bytes_icon_new(bytes);
    if (icon == NULL) {
        g_printerr("Error loading file as icon...\n");
        // TODO set returned error

        g_bytes_unref(bytes);
        g_object_unref(file);
        return NULL;
    }
    GVariant* serialized_icon = g_icon_serialize(icon);

    g_print("Icon serialized successfully\n");

    g_object_unref(icon);
    g_bytes_unref(bytes);
    g_object_unref(file);

    return serialized_icon;
}

// This callback is run by libportal when the user has confirmed or cancelled installing the desktop entry
static void install_callback (void *source, GAsyncResult *result, InstallCallbackData *data) {
    // Get result
    g_print("Calling xdp_portal_dynamic_launcher_prepare_install_finish()\n");
    GError *error = NULL;
    GVariant *result_v = xdp_portal_dynamic_launcher_prepare_install_finish(portal, result, &error);
    if (error != NULL) {
        g_printerr("prepare_install_finish returned an error: %s\n", error->message);
        free(data);
        exit(1);
    }

    // result_v is a dictionary with "name", "token" and "icon"
    GVariant *name_v = g_variant_lookup_value(result_v, "name", G_VARIANT_TYPE_STRING);
    GVariant *token_v = g_variant_lookup_value(result_v, "token", G_VARIANT_TYPE_STRING);
    if (name_v == NULL) {
        g_printerr("prepare_install did not return a \"name\"...\n");
        free(data);
        g_variant_unref(result_v);
        exit(1);
    }
    if (token_v == NULL) {
        g_printerr("prepare_install did not return a \"token\"...\n");
        free(data);
        g_variant_unref(result_v);
        exit(1);
    }

    const gchar *name = g_variant_get_string(name_v, NULL);
    const gchar *token = g_variant_get_string(token_v, NULL);

    // Done with these GVariants now
    g_variant_unref(result_v);
    g_variant_unref(name_v);
    g_variant_unref(token_v);

    g_print("token: %s\ndesktop_entry_name: %s\ndesktop_entry_id: %s\ndesktop_entry_content: %s\n",
    token, name, data->desktop_entry_id, data->desktop_entry_content);

    // Install desktop entry via libportal
    g_print("Calling xdp_portal_dynamic_launcher_install()\n");
    xdp_portal_dynamic_launcher_install(
        portal,
        token,
        data->desktop_entry_id,
        data->desktop_entry_content,
        &error
    );

    if (error != NULL) {
        g_printerr("Unable to install desktop entry... Error: %s\n", error->message);
        free(data);
        exit(1);
    }

    g_print("Successfully installed desktop entry \"%s\"\n", data->desktop_entry_id);
    free(data);
    g_main_loop_quit(loop);
}

void install(gchar* desktop_entry_id, gchar* name, gchar* icon_filename, gchar* desktop_entry_content, gboolean name_editable, gboolean icon_editable) {
    GError *error = NULL;
    // Check for valid options
    // TODO - name, filename, etc must be set
    // TODO check for up to one option bein "-" (read from stdin)

    // Populate callback data
    // Assigning heap memory so we can keep data around until the callback runs
    InstallCallbackData *data = malloc(sizeof (InstallCallbackData));
    if (data == NULL) {
        g_printerr("Error allocating memory for install callback data\n");
        exit(1);
    }
    data->desktop_entry_id = desktop_entry_id;
    data->desktop_entry_content = desktop_entry_content;

    // Load icon and serialize it
    GVariant *icon_v = load_icon(icon_filename, &error);

    if (error != NULL) {
        g_printerr("Error serializing icon: %s", error->message);
        free(data);
        exit(1);
    } else if (icon_v == NULL) {
        // FIXME this second case shouldn't be needed, but load_icon() doesn't set error in all cases yet!
        g_printerr("Error serializing icon");
        free(data);
        exit(1);
    }

    // Ask libportal for a token to install the desktop entry
    // This will open a dialog asking the user to confirm (and possibly edit name/icon)
    // When the dialog is closed, install_callback() will be run
    // data will be passed to install_callback(), along with the chosen name & icon.
    xdp_portal_dynamic_launcher_prepare_install(
        portal,
        NULL, // parent
        name,
        icon_v,
        XDP_LAUNCHER_APPLICATION, // TODO XDP_LAUNCHER_WEBAPP option
        NULL, // TODO XDP_LAUNCHER_WEBAPP option
        name_editable,
        icon_editable,
        NULL, // cancellable
        (GAsyncReadyCallback) install_callback,
        data
    );

    g_variant_unref(icon_v);
}

void uninstall(gchar* desktop_entry_id) {
    g_printerr("Error: uninstall command is not implemented yet!\n");
    exit(1);
}

void get_desktop_entry(gchar* desktop_entry_id) {
    g_printerr("Error: get command is not implemented yet!\n");
    exit(1);
}

void get_icon_from_desktop_entry(gchar* desktop_entry_id, gchar* icon_filename) {
    g_printerr("Error: geticon command is not implemented yet!\n");
    exit(1);
}

void launch_desktop_entry(gchar* desktop_entry_id) {
    g_printerr("Error: launch command is not implemented yet!\n");
    exit(1);
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

    // Get an instance of XdpPortal
    portal = xdp_portal_new();

    // Run the relevent command, passing in relevent options
    gchar* command = g_ascii_strdown(g_strstrip(argv[1]), -1);
    if (strcmp(command, "install")==0) {
        install(entry_id, entry_name, icon_filename, entry, name_editable, icon_editable);
    } else if (strcmp(command, "uninstall")==0) {
        uninstall(entry_id);
    } else if (strcmp(command, "get")==0) {
        get_desktop_entry(entry_id);
    } else if (strcmp(command, "geticon")==0) {
        get_icon_from_desktop_entry(entry_id, icon_filename);
    } else if (strcmp(command, "launch")==0) {
        launch_desktop_entry(entry_id);
    } else {
        g_printerr("Error: Unrecognised command \"%s\"\n", argv[1]);
        return 1;
    }

    // Run the main event loop:
    g_print("Running the main event loop\n");
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    // Anything here will runn after g_main_loop_quit(loop)

    return 0;
}