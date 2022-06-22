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

gchar *get_help_from_portal(Portal *portal) {
    gchar *prog_name = "xdg-portal";//FIXME get from argv - maybe set a global in main.c?
    g_autoptr(GString) string = NULL;
    string = g_string_sized_new(512);

    // Usage
    g_string_append_printf (string, "Usage: %s %s <command> [OPTION]...\n", prog_name, portal->name);
    for (int i = 0; i < CLI_MAX_ALIASES; i++) {
        if (portal->aliases[i] == NULL || portal->aliases[i][0] == '\0')
        {
            break;
        }

        g_string_append_printf (string, "  or:  %s %s <command> [OPTION]...\n", prog_name, portal->aliases[i]);
    }

    g_string_append_printf(string, "\n%s\n\nTo view options for a given command, run the command with the --help option.\n\n", portal->help_text);

    // Loop through all commands to find the longest
    gsize longest_command = 0;
    for (int i = 0; i < CLI_PORTAL_MAX_COMMANDS; i++) {
        if (portal->commands[i].name == NULL)
        {
            continue;
        }

        gsize len = strlen(portal->commands[i].name);
        if (len > longest_command) longest_command = len;
    }

    // Loop through again, printing each command this time
    for (int i = 0; i < CLI_PORTAL_MAX_COMMANDS; i++) {
        if (portal->commands[i].name == NULL)
        {
            continue;
        }

        g_string_append_printf(string, "    %s", portal->commands[i].name);

        gsize len = strlen(portal->commands[i].name);
        gsize padding = 8 + longest_command - len;

        while (padding)
        {
            padding--;
            g_string_append_c(string, ' ');
        }

        g_string_append(string, portal->commands[i].help_text);
        g_string_append_c(string, '\n');

        if (len > longest_command) longest_command = len;
        for (int j = 0; j < CLI_MAX_ALIASES; j++) {
            if (portal->commands[i].aliases[j] == NULL || portal->commands[i].aliases[j][0] == '\0')
            { // FIXME Code duplication
                if (j > 0) g_string_append_c(string, '\n');
                break;
            }

            if (j == 0) {
                g_string_append(string, "\n      Aliases: ");
            } else {
                g_string_append(string, ", ");
            }
            g_string_append(string, portal->commands[i].aliases[j]);
            len = strlen (portal->commands[i].aliases[j]);

            if (j+1 == CLI_MAX_ALIASES)
            { // FIXME Code duplication
                if (j > 0) g_string_append_c(string, '\n');
            }
        }
    }

    g_string_append(string, "\nFooter text here?\n");

    // Copy retult into heap memory so we can free the GString
    gchar *return_string = malloc(string->len);
    if (return_string == NULL)
    {
        g_printerr ("Error assigning memory to get_help_from_portal's return value");
        return NULL;
    }
    memcpy (return_string, string->str, string->len);
    return return_string;
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

gchar *get_help_from_command(Command *command)
{
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
