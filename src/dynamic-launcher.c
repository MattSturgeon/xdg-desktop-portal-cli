#include "dynamic-launcher.h"

#include "command.h"
#include "event-loop.h"
#include "xdg-portal.h"

static gboolean icon_editable = FALSE;
static gboolean name_editable = FALSE;
static gchar*   icon_filename = NULL;
static gchar*   entry_name    = NULL;
static gchar*   entry_content = NULL;

static Command install_command = {
    .name  = "install",
    .usage = "<desktop_file_name>",
    .help_text =
        "TODO: copy from docs... installs a desktop entry onto the host "
        "system...",
    .handler = &dynamic_launcher_install,
    .options =
        {
            {
                "icon-editable",
                0,
                0,
                G_OPTION_ARG_NONE,
                &icon_editable,
                "Whether or not the user will be able edit the icon. Used when "
                "installing a new desktop entry.",
                NULL,
            },
            {
                "name-editable",
                0,
                0,
                G_OPTION_ARG_NONE,
                &name_editable,
                "Whether or not the user will be able edit the name of the "
                "desktop "
                "entry. Used when installing a new desktop entry.",
                NULL,
            },
            {
                "icon-file",
                'i',
                0,
                G_OPTION_ARG_FILENAME,
                &icon_filename,
                "Path to an app icon. Either the icon to use when installing, "
                "or "
                "the "
                "path to save the icon when running geticon.",
                "FILE",
            },
            {
                "name",
                0,
                0,
                G_OPTION_ARG_STRING,
                &entry_name,
                "The name of the dekstop entry. Used when installing a new "
                "desktop "
                "entry.",
                "NAME",
            },
            {
                "content",
                0,
                0,
                G_OPTION_ARG_STRING,
                &entry_content,
                "The actual content of the desktop entry. Used when installing "
                "a "
                "new "
                "desktop entry. Some parts may be replaced or edited by the "
                "portal,\n"
                "                           in particular Icon, Name and Exec.",
                "CONTENT",
            },
            {NULL},
        },
};

static Command uninstall_command = {
    .name = "uninstall",
    .help_text =
        "TODO: copy from docs... removes a "
        "desktop entry from the host system...",
    .options =
        {
            // TODO
            {NULL},
        },
};

static Command get_command = {
    .name = "get",
    .help_text =
        "TODO: copy from docs... gets a desktop entry "
        "from the host system...",
    .options =
        {
            // TODO
            {NULL},
        },
};

static Command get_icon_command = {
    .name = "get_icon",
    .help_text =
        "TODO: copy from docs... gets the icon from a desktop entry on the "
        "host system...",
    .options =
        {
            // TODO
            {NULL},
        },
};

static Command launch_command = {
    .name = "launch",
    .help_text =
        "TODO: copy from docs... runs a desktop "
        "entry found on the host system...",
    .options =
        {
            // TODO
            {NULL},
        },
};

Portal dynamic_launcher_portal = {
    .name    = "dynamic-launcher",
    .aliases = {"launcher", "desktop-entry"},
    .help_text =
        "TODO: Copy paste from docs... describe "
        "what the dynamic-launcher portal does...",
    .commands =
        {
            &install_command,
            &uninstall_command,
            &get_command,
            &get_icon_command,
            &launch_command,
        },
};

// Datatype used to pass data from install() to install_callback()
typedef struct _InstallCallbackData {
  // TODO Consider what data needs to be passed to callback(s)
  gchar* desktop_entry_id;
  gchar* desktop_entry_content;
} InstallCallbackData;

gboolean str_ends_with(const gchar* str, const gchar* suffix)
{
  if (!str || !suffix)
    return FALSE;

  gsize lenstr    = strlen(str);
  gsize lensuffix = strlen(suffix);
  if (lensuffix > lenstr)
    return FALSE;

  return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

// This callback is run by libportal when the user has confirmed or cancelled
// installing the desktop entry
static void install_callback(void*                source,
                             GAsyncResult*        result,
                             InstallCallbackData* data)
{
  // Get result
  g_print("Calling xdp_portal_dynamic_launcher_prepare_install_finish()\n");
  GError*   error    = NULL;
  GVariant* result_v = xdp_portal_dynamic_launcher_prepare_install_finish(
      get_xdg_portal(), result, &error);
  if (error != NULL) {
    g_printerr("prepare_install_finish returned an error: %s\n",
               error->message);
    free(data);
    exit(1);
  }

  // result_v is a dictionary with "name", "token" and "icon"
  GVariant* name_v =
      g_variant_lookup_value(result_v, "name", G_VARIANT_TYPE_STRING);
  GVariant* token_v =
      g_variant_lookup_value(result_v, "token", G_VARIANT_TYPE_STRING);
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

  const gchar* name  = g_variant_get_string(name_v, NULL);
  const gchar* token = g_variant_get_string(token_v, NULL);

  // Done with these GVariants now
  g_variant_unref(result_v);
  g_variant_unref(name_v);
  g_variant_unref(token_v);

  g_print(
      "token: %s\ndesktop_entry_name: %s\ndesktop_entry_id: "
      "%s\ndesktop_entry_content: %s\n",
      token,
      name,
      data->desktop_entry_id,
      data->desktop_entry_content);

  // Install desktop entry via libportal
  g_print("Calling xdp_portal_dynamic_launcher_install()\n");
  xdp_portal_dynamic_launcher_install(get_xdg_portal(),
                                      token,
                                      data->desktop_entry_id,
                                      data->desktop_entry_content,
                                      &error);

  if (error != NULL) {
    g_printerr("Unable to install desktop entry... Error: %s\n",
               error->message);
    free(data);
    exit(1);
  }

  g_print("Successfully installed desktop entry \"%s\"\n",
          data->desktop_entry_id);
  free(data);
  finish_loop();
}

// Loads the icon into memory and serializes it into a GVariant
GVariant* load_icon(char* filename, GError** error)
{
  // Load the icon
  // TODO add some checks to make sure we don't load a ridiculously big icon
  // into memory
  GError* internal_error = NULL;
  gchar*  etag;
  GFile*  file  = g_file_new_for_path(filename);
  GBytes* bytes = g_file_load_bytes(file, NULL, &etag, &internal_error);
  if (internal_error != NULL) {
    g_printerr("Error loading file into memory: %s\n", internal_error->message);
    *error = internal_error;

    g_bytes_unref(bytes);
    g_object_unref(file);
    return NULL;
  }
  g_print("File loaded... etag: %s\n", etag);
  g_print("%ld bytes read\n", g_bytes_get_size(bytes));

  // Serialize the icon
  GIcon* icon = g_bytes_icon_new(bytes);
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

gint dynamic_launcher_install(gint argc, gchar* argv[])
{
  GError* error            = NULL;
  gchar*  desktop_entry_id = NULL;

  if (argc < 1) {
    g_printerr("desktop_entry filename not specified\n");
    return EXIT_FAILURE;
  }
  desktop_entry_id = argv[0];
  if (!str_ends_with(desktop_entry_id, ".desktop")) {
    g_printerr("desktop_entry filename must end with \"%s\"\n", ".desktop");
    return EXIT_FAILURE;
  }

  // Check for valid options
  // TODO - name, filename, etc must be set
  // TODO check for up to one option bein "-" (read from stdin)

  // Populate callback data
  // Assigning heap memory so we can keep data around until the callback runs
  InstallCallbackData* data = malloc(sizeof(InstallCallbackData));
  if (data == NULL) {
    g_printerr("Error allocating memory for install callback data\n");
    return EXIT_FAILURE;
  }
  data->desktop_entry_id      = desktop_entry_id;
  data->desktop_entry_content = entry_content;

  // Load icon and serialize it
  GVariant* icon_v = load_icon(icon_filename, &error);

  if (error != NULL) {
    g_printerr("Error serializing icon: %s", error->message);
    free(data);
    return EXIT_FAILURE;
  } else if (icon_v == NULL) {
    // FIXME this second case shouldn't be needed, but load_icon() doesn't set
    // error in all cases yet!
    g_printerr("Error serializing icon");
    free(data);
    return EXIT_FAILURE;
  }

  // Ask libportal for a token to install the desktop entry
  // This will open a dialog asking the user to confirm (and possibly edit
  // name/icon) When the dialog is closed, install_callback() will be run data
  // will be passed to install_callback(), along with the chosen name & icon.
  xdp_portal_dynamic_launcher_prepare_install(
      get_xdg_portal(),
      NULL,  // parent
      entry_name,
      icon_v,
      XDP_LAUNCHER_APPLICATION,  // TODO XDP_LAUNCHER_WEBAPP option
      NULL,                      // TODO XDP_LAUNCHER_WEBAPP option
      name_editable,
      icon_editable,
      NULL,  // cancellable
      (GAsyncReadyCallback)install_callback,
      data);

  g_variant_unref(icon_v);

  // Enable GLib's event loop, since we need to wait for install_callback to run
  enable_loop();
  return EXIT_SUCCESS;
}
