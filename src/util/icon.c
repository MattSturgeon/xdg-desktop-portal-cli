#include "util/icon.h"

#include <gio/gio.h>

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