#include <glib.h>

typedef struct _PORTAL Portal;
typedef struct _COMMAND Command;

// Define our command callback function pointer
// Takes any remaining argc & argv
// Returns the exit status code
typedef gint (*CommandCallback)(gint, gchar*[], gboolean show_help);

// Function pointer to handle portal
// Takes any remaining argc & argv
// Returns the chosen command
typedef Command* (*PortalCallback)(gint, gchar*[], gboolean show_help);

Portal *get_portal_from_string(gchar* portal_string);
gchar *get_help_from_portal(Portal *portal);
Command *call_portal_handler(Portal *portal, gint argc, gchar** argv, gboolean show_help);
gchar *get_portal_name(Portal *portal);

Command *get_command_from_string(Portal *portal, gchar* command_string);
gchar *get_help_from_command(Command *command);
gint call_command_handler(Command *command, gint argc, gchar** argv, gboolean show_help);
gchar *get_command_name(Command *command);
