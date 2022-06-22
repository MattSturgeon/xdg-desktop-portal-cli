#include "command.h"
#include "dynamic-launcher.h"

#define CLI_MAX_ALIASES 3
#define CLI_PORTAL_MAX_COMMANDS 10
#define CLI_MAX_OPTIONS 10

struct _COMMAND {
    gchar* name;
    gchar* aliases[CLI_MAX_ALIASES];
    gchar* help_text;
    GOptionEntry options[CLI_MAX_OPTIONS];
    CommandCallback handler;
};

struct _PORTAL {
    gchar *name;
    gchar *aliases[CLI_MAX_ALIASES];
    gchar *help_text;
    Command commands[CLI_PORTAL_MAX_COMMANDS];
    PortalCallback handler;
};

static Portal cli_sturcture[] = {
    {
        .name = "dynamic-launcher",
        .aliases = { "launcher", "desktop-entry" },
        .help_text = "TODO: Copy paste from docs... describe what the dynamic-launcher portal does...",
        .commands = {
            {
                .name = "install",
                .help_text = "TODO: copy from docs... installs a desktop entry onto the host system...",
                .handler = &dynamic_launcher_install,
                .options = {
                    // TODO
                    { NULL },
                }
            },
            {
                .name = "uninstall",
                .help_text = "TODO: copy from docs... removes a desktop entry from the host system...",
                .options = {
                    // TODO
                    { NULL },
                }
            },
            {
                .name = "get",
                .help_text = "TODO: copy from docs... gets a desktop entry from the host system...",
                .options = {
                    // TODO
                    { NULL },
                }
            },
            {
                .name = "get_icon",
                .help_text = "TODO: copy from docs... gets the icon from a desktop entry on the host system...",
                .options = {
                    // TODO
                    { NULL },
                }
            },
            {
                .name = "launch",
                .help_text = "TODO: copy from docs... runs a desktop entry found on the host system...",
                .options = {
                    // TODO
                    { NULL },
                }
            },
        }
    },
};

Portal *get_portal_from_string(gchar* portal_string)
{
    gchar* clean_portal_string = g_ascii_strdown(g_strstrip(portal_string), -1);
    gint cli_len = sizeof(cli_sturcture) / sizeof(cli_sturcture[0]);
    for (int i = 0; i < cli_len; i++)
    {
        if (cli_sturcture[i].name == NULL || cli_sturcture[i].name[0] == '\0')
        {
            g_printerr("Error: Portal at index %d does not have a name set\n", i);
        }
        else if (strcmp(clean_portal_string, cli_sturcture[i].name) == 0)
        {
            return &cli_sturcture[i];
        }

        for (int j = 0; j < CLI_MAX_ALIASES; j++)
        {
            if (cli_sturcture[i].aliases[j] == NULL || cli_sturcture[i].aliases[j][0] == '\0')
            {
                break;
            }

            if (strcmp(clean_portal_string, cli_sturcture[i].aliases[j]) == 0)
                return &cli_sturcture[i];
        }
    }
    return NULL;
}

Command *call_portal_handler(Portal *portal, gint argc, gchar** argv, gboolean show_help)
{
    return portal->handler(argc, argv, show_help);

}

gchar *get_portal_name(Portal *portal) {
    return portal->name;
}

Command *get_command_from_string(Portal *portal, gchar *command_string)
{
    gchar* clean_command_string = g_ascii_strdown(g_strstrip(command_string), -1);
    for (int i = 0; i < CLI_PORTAL_MAX_COMMANDS; i++)
    {
        if (portal->commands[i].name == NULL || portal->commands[i].name[0] == '\0')
        {
            g_printerr("Error: %s command at index %d does not have a name set\n", portal->name, i);
        }
        else if (strcmp(clean_command_string, portal->commands[i].name) == 0)
        {
            return &portal->commands[i];
        }

        for (int j = 0; j < CLI_MAX_ALIASES; j++)
        {
            if (portal->commands[i].aliases[j] == NULL || portal->commands[i].aliases[j][0] == '\0')
            {
                break;
            }

            if (strcmp(clean_command_string, portal->commands[i].aliases[j]) == 0)
                return &portal->commands[i];
        }
    }
    return NULL;
}

gint call_command_handler(Command *command, gint argc, gchar** argv, gboolean show_help)
{
    return command->handler(argc, argv, show_help);
}

gchar *get_command_name(Command *command)
{
    return command->name;
}
