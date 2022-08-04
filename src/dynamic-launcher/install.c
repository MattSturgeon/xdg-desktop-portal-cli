#include "dynamic-launcher.h"
#include "event-loop.h"
#include "util/icon.h"
#include "util/unused.h"
#include "xdg-portal.h"

static gboolean icon_editable = FALSE;
static gboolean name_editable = FALSE;
static gchar*   icon_filename = NULL;
static gchar*   entry_name    = NULL;
static gchar*   entry_content = NULL;

gint install_command_handler(gint argc, gchar* argv[]);

Command install_command = {
    .name    = "install",
    .handler = &install_command_handler,
    .usage   = "<desktop_file_name>",
    .help_text =
        "Install a new desktop file and icon into the appropriate directories "
        "to allow the desktop environment to find them. A dialog is shown to "
        "the user asking them to confirm installing the desktop file.\n"
        "Note that any existing desktop file with the same name will be "
        "overwritten, use the get command to check if one already exists.\n"
        "The desktop_file_name must have \".desktop\" as a suffix. Except in "
        "the special case when the calling process has no associated app ID,"
        "desktop_file_name must start with the app ID followed by a period.",
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
                "desktop entry. Used when installing a new desktop entry.",
                NULL,
            },
            // TODO support base64 icon input
            {
                "icon-file",
                'i',
                0,
                G_OPTION_ARG_FILENAME,
                &icon_filename,
                "Path to an app icon. Either the icon to use when installing, "
                "or the path to save the icon when running geticon.",
                "FILE",
            },
            {
                "name",
                0,
                0,
                G_OPTION_ARG_STRING,
                &entry_name,
                "The name of the dekstop entry. Used when installing a new "
                "desktop entry.",
                "NAME",
            },
            {
                "content",
                0,
                0,
                G_OPTION_ARG_STRING,
                &entry_content,
                "The actual content of the desktop entry. Used when installing "
                "a new desktop entry. Some parts may be replaced or edited by "
                "the portal, in particular Icon, Name and Exec.",
                "CONTENT",
            },
            {NULL},
        },
};

// Datatype used to pass data from install() to install_callback()
typedef struct _InstallCallbackData {
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
static void install_callback(void*                UNUSED(source),
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

gint install_command_handler(gint argc, gchar* argv[])
{
  GError*  error            = NULL;
  gchar*   desktop_entry_id = NULL;
  gboolean valid            = TRUE;

  // Check for valid arguments
  if (argc < 1) {
    g_printerr("desktop_entry filename not specified\n");
    valid = FALSE;
  }

  if (valid) {
    desktop_entry_id = argv[0];
    if (!str_ends_with(desktop_entry_id, ".desktop")) {
      g_printerr("desktop_entry filename must end with \"%s\"\n", ".desktop");
      valid = FALSE;
    }
  }

  // Check for valid options
  if (icon_filename == NULL) {
    g_printerr("icon filename not provided\n");
    valid = FALSE;
  }
  if (entry_name == NULL) {
    g_printerr("entry name not provided\n");
    valid = FALSE;
  }
  if (entry_content == NULL) {
    g_printerr("entry content not provided\n");
    valid = FALSE;
  }

  if (!valid)
    return EXIT_FAILURE;

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
