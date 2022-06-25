#pragma once
#include <glib.h>

#define CLI_MAX_ALIASES 3
#define CLI_PORTAL_MAX_COMMANDS 10
#define CLI_MAX_OPTIONS 10


// Define our command callback function pointer
// Takes any remaining argc & argv
// Returns the exit status code
typedef gint (*CommandCallback)(gint, gchar*[]);

typedef struct _COMMAND {
    gchar* name;
    gchar* aliases[CLI_MAX_ALIASES];
    gchar* help_text;
    gchar* usage;
    GOptionEntry options[CLI_MAX_OPTIONS];
    CommandCallback handler;
} Command;


// Function pointer to handle portal
// Takes any remaining argc & argv
// Returns the chosen command
typedef Command* (*PortalCallback)(gint, gchar*[]);

typedef struct _PORTAL {
    gchar *name;
    gchar *aliases[CLI_MAX_ALIASES];
    gchar *help_text;
    Command *commands[CLI_PORTAL_MAX_COMMANDS];
    PortalCallback handler;
} Portal;

gchar *get_help();

Portal *get_portal_from_string(gchar* portal_string);
gchar *get_help_from_portal(Portal *portal);
Command *call_portal_handler(Portal *portal, gint argc, gchar** argv);
gchar *get_portal_name(Portal *portal);

Command *get_command_from_string(Portal *portal, gchar* command_string);
gchar *get_help_from_command(Command *command);
gint call_command_handler(Command *command, gint argc, gchar** argv);
gchar *get_command_name(Command *command);



