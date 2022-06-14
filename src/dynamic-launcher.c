#define GETTEXT_PACKAGE "gtk20"

#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>

#define PROGRAM_DESCRIPTION "wrapper for select libportal APIs"
#define ABOUT "wrapper for select libportal APIs"

// Main options
static gboolean version = FALSE;
static gboolean verbose = FALSE;
static gchar* icon_filename = "";
static gchar* entry_id = "";
static gchar* entry = "";

static GOptionEntry entries[] =
    {
        {"verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL},
        {"version", 0, 0, G_OPTION_ARG_NONE, &version, "Show version information", NULL},
        {"icon", 'i', 0, G_OPTION_ARG_FILENAME, &icon_filename, "Application icon", NULL},
        {"id", 0, 0, G_OPTION_ARG_STRING, &entry_id, "Desktop entry id or app id", NULL},
        {"entry", 0, 0, G_OPTION_ARG_STRING, &entry, "Desktop entry (the actual content of the desktop file).\nSome parts may be replaced or edited by the portal, in particular Icon, Name and Exec.", NULL},
        {NULL}
    };


int install() {
    return 0;
}

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
    GFileIcon *icon = g_file_icon_new(file);
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

int main(int argc, char *argv[])
{
    GError *error = NULL;
    GOptionContext *context = g_option_context_new("<command> ");
    g_option_context_set_summary(context, "This is the summary");
    g_option_context_set_description(context, "This is the description");
    g_option_context_add_main_entries(context, entries, GETTEXT_PACKAGE);

    if (!g_option_context_parse(context, &argc, &argv, &error))
    {
        g_printerr("Error: %s\n", error->message);
        exit(1);
    }
    g_option_context_free (context);

	if (version) {
		g_print ("\n" ABOUT "\n" "LICENSE" "\n");
		exit(0);
	}

    g_print("Args count: %d\n", argc);
    for (gsize i = 0; i < argc; i++)
    {
        g_print("  \"%s\"\n", argv[i]);
    }

    if (icon_filename[0] != '\0') {
        g_print("Icon filename: %s\n", icon_filename);
        // TODO check icon size
        // TODO load icon
        GVariant* serialized_icon = load_icon(icon_filename, error);
        // TODO move variable scope somewhere useful
    }

    if (entry_id[0] != '\0') {
        g_print("Desktop Entry ID: %s\n", entry_id);
    }

    if (entry[0] != '\0') {
        g_print("Desktop Entry: %s\n", entry);
    }

    if (argc > 1 && strcmp("install", argv[1])) {
        return install();
    }

    return 0;
}