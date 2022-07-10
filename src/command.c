#include "command.h"

#include <sys/ioctl.h>
#include <unistd.h>

#include "dynamic-launcher.h"

static Portal* cli_sturcture[] = {
    &dynamic_launcher_portal,
};

gchar* get_help()
{
  // FIXME get from argv - maybe set a global in main.c?
  gchar* prog_name          = "xdg-portal";
  g_autoptr(GString) string = NULL;
  string                    = g_string_sized_new(512);

  // Usage
  g_string_append_printf(
      string, "Usage: %s <portal> <command> [OPTION]...\n\n", prog_name);

  g_string_append(string, "TODO: Main help text...\n\n");

  g_string_append_printf(string,
                         "To view options for a given portal or command, use: "
                         "%s <portal> --help\n\n",
                         prog_name);

  g_string_append(string, "Available Portals:\n");

  // Loop through all portals to find the longest
  gsize longest_portal = 0;
  gint  cli_len        = sizeof(cli_sturcture) / sizeof(cli_sturcture[0]);
  for (int i = 0; i < cli_len; i++) {
    if (cli_sturcture[i] == NULL)
      break;

    if (cli_sturcture[i]->name == NULL)
      continue;

    gsize len = strlen(cli_sturcture[i]->name);
    if (len > longest_portal)
      longest_portal = len;
  }

  // Loop through again, printing each portal this time
  for (int i = 0; i < cli_len; i++) {
    if (cli_sturcture[i] == NULL)
      break;

    if (cli_sturcture[i]->name == NULL)
      continue;

    g_string_append_printf(string, "  %s", cli_sturcture[i]->name);

    gsize len     = strlen(cli_sturcture[i]->name);
    gsize padding = 8 + longest_portal - len;

    while (padding) {
      padding--;
      g_string_append_c(string, ' ');
    }

    g_string_append(string, cli_sturcture[i]->help_text);
    g_string_append_c(string, '\n');

    for (int j = 0; j < CLI_MAX_ALIASES; j++) {
      if (cli_sturcture[i]->aliases[j] == NULL
          || cli_sturcture[i]->aliases[j][0]
                 == '\0') {  // FIXME Code duplication
        if (j > 0)
          g_string_append_c(string, '\n');
        break;
      }

      if (j == 0)
        g_string_append(string, "      Aliases: ");
      else
        g_string_append(string, ", ");
      g_string_append(string, cli_sturcture[i]->aliases[j]);
      len = strlen(cli_sturcture[i]->aliases[j]);

      if (j + 1 == CLI_MAX_ALIASES) {  // FIXME Code duplication
        if (j > 0)
          g_string_append_c(string, '\n');
      }
    }
  }

  g_string_append(string, "\nFooter text here?\n");

  // Copy retult into heap memory so we can free the GString
  gchar* return_string = malloc(string->len);
  if (return_string == NULL) {
    g_printerr("Error assigning memory to get_help_from_portal's return value");
    return NULL;
  }
  memcpy(return_string, string->str, string->len);
  return return_string;
}

Portal* get_portal_from_string(gchar* portal_string)
{
  gchar* clean_portal_string = g_ascii_strdown(g_strstrip(portal_string), -1);
  gint   cli_len             = sizeof(cli_sturcture) / sizeof(cli_sturcture[0]);
  for (int i = 0; i < cli_len; i++) {
    if (cli_sturcture[i] == NULL) {
      // Found the end of the array
      break;
    }
    if (cli_sturcture[i]->name == NULL || cli_sturcture[i]->name[0] == '\0')
      g_printerr("Error: Portal at index %d does not have a name set\n", i);

    else if (strcmp(clean_portal_string, cli_sturcture[i]->name) == 0)
      return cli_sturcture[i];

    for (int j = 0; j < CLI_MAX_ALIASES; j++) {
      if (cli_sturcture[i]->aliases[j] == NULL
          || cli_sturcture[i]->aliases[j][0] == '\0')
        break;

      if (strcmp(clean_portal_string, cli_sturcture[i]->aliases[j]) == 0)
        return cli_sturcture[i];
    }
  }
  return NULL;
}

gchar* get_help_from_portal(Portal* portal)
{
  gchar* prog_name =
      "xdg-portal";  // FIXME get from argv - maybe set a global in main.c?
  g_autoptr(GString) string = NULL;
  string                    = g_string_sized_new(512);

  // Usage
  g_string_append_printf(
      string, "Usage: %s %s <command> [OPTION]...\n", prog_name, portal->name);
  for (int i = 0; i < CLI_MAX_ALIASES; i++) {
    if (portal->aliases[i] == NULL || portal->aliases[i][0] == '\0')
      break;

    g_string_append_printf(string,
                           "  or:  %s %s <command> [OPTION]...\n",
                           prog_name,
                           portal->aliases[i]);
  }

  g_string_append_printf(string, "\n%s\n\n", portal->help_text);
  g_string_append(string,
                  "To view options for a given command, run the command with "
                  "the --help option.\n\n");
  g_string_append(string, "Available Commands:\n");

  // Loop through all commands to find the longest
  gsize longest_command = 0;
  for (int i = 0; i < CLI_PORTAL_MAX_COMMANDS; i++) {
    if (portal->commands[i] == NULL)
      break;

    if (portal->commands[i]->name == NULL)
      continue;

    gsize len = strlen(portal->commands[i]->name);
    if (len > longest_command)
      longest_command = len;
  }

  // Loop through again, printing each command this time
  for (int i = 0; i < CLI_PORTAL_MAX_COMMANDS; i++) {
    if (portal->commands[i] == NULL)
      break;

    if (portal->commands[i]->name == NULL)
      continue;

    g_string_append_printf(string, "  %s", portal->commands[i]->name);

    gsize len     = strlen(portal->commands[i]->name);
    gsize padding = 8 + longest_command - len;

    while (padding) {
      padding--;
      g_string_append_c(string, ' ');
    }

    g_string_append(string, portal->commands[i]->help_text);
    g_string_append_c(string, '\n');

    for (int j = 0; j < CLI_MAX_ALIASES; j++) {
      if (portal->commands[i]->aliases[j] == NULL
          || portal->commands[i]->aliases[j][0]
                 == '\0') {  // FIXME Code duplication
        if (j > 0)
          g_string_append_c(string, '\n');
        break;
      }

      if (j == 0)
        g_string_append(string, "      Aliases: ");
      else
        g_string_append(string, ", ");
      g_string_append(string, portal->commands[i]->aliases[j]);
      len = strlen(portal->commands[i]->aliases[j]);

      if (j + 1 == CLI_MAX_ALIASES) {  // FIXME Code duplication
        if (j > 0)
          g_string_append_c(string, '\n');
      }
    }
  }

  g_string_append(string, "\nFooter text here?\n");

  // Copy retult into heap memory so we can free the GString
  gchar* return_string = malloc(string->len);
  if (return_string == NULL) {
    g_printerr("Error assigning memory to get_help_from_portal's return value");
    return NULL;
  }
  memcpy(return_string, string->str, string->len);
  return return_string;
}

Command* call_portal_handler(Portal* portal, gint argc, gchar** argv)
{
  return portal->handler(argc, argv);
}

gchar* get_portal_name(Portal* portal)
{
  return portal->name;
}

gchar* get_option_left_text(GOptionEntry* option)
{
  if (option == NULL || option->long_name == NULL)
    return NULL;

  g_autoptr(GString) string = NULL;
  string                    = g_string_sized_new(128);

  g_string_append(string, "  ");

  if (option->short_name)
    g_string_append_printf(string, "-%c, ", option->short_name);

  g_string_append_printf(string, "--%s", option->long_name);

  if (option->arg_description)
    g_string_append_printf(string, "=%s", option->arg_description);

  g_string_append_c(string, '\0');

  // Copy retult into heap memory so we can free the GString
  gchar* return_string = malloc(string->len);
  if (return_string == NULL) {
    g_printerr("Error assigning memory to get_option_left_text's return value");
    return NULL;
  }
  memcpy(return_string, string->str, string->len);
  return return_string;
}

Command* get_command_from_string(Portal* portal, gchar* command_string)
{
  gchar* clean_command_string = g_ascii_strdown(g_strstrip(command_string), -1);
  for (int i = 0; i < CLI_PORTAL_MAX_COMMANDS; i++) {
    if (portal->commands[i]->name == NULL
        || portal->commands[i]->name[0] == '\0') {
      g_printerr("Error: %s command at index %d does not have a name set\n",
                 portal->name,
                 i);
    } else if (strcmp(clean_command_string, portal->commands[i]->name) == 0) {
      return portal->commands[i];
    }

    for (int j = 0; j < CLI_MAX_ALIASES; j++) {
      if (portal->commands[i]->aliases[j] == NULL
          || portal->commands[i]->aliases[j][0] == '\0')
        break;

      if (strcmp(clean_command_string, portal->commands[i]->aliases[j]) == 0)
        return portal->commands[i];
    }
  }
  return NULL;
}

unsigned short int get_columns()
{
  struct winsize ws;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
  return ws.ws_col;
}

Portal* get_command_owner(Command* command)
{
  gint cli_len = sizeof(cli_sturcture) / sizeof(cli_sturcture[0]);
  for (int i = 0; i < cli_len; i++) {
    if (cli_sturcture[i] == NULL)
      break;

    for (int j = 0; j < CLI_PORTAL_MAX_COMMANDS; j++) {
      if (cli_sturcture[i]->commands[j] == NULL)
        break;
      if (command == cli_sturcture[i]->commands[j])
        return cli_sturcture[i];
    }
  }
  return NULL;
}

gchar* get_help_from_command(Command* command)
{
  gchar* prog_name =
      "xdg-portal";  // FIXME get from argv - maybe set a global in main.c?
  gsize columns             = get_columns();
  g_autoptr(GString) string = NULL;
  string                    = g_string_sized_new(512);

  // Find portal that owns command
  Portal* owner = get_command_owner(command);

  // Usage
  // TODO only print "[OPTION]..." if Command has options
  g_string_append_printf(string,
                         "Usage: %s %s %s [OPTION]... %s\n",
                         prog_name,
                         owner->name,
                         command->name,
                         command->usage);

  g_string_append_printf(string, "\n%s\n\nOptions:\n", command->help_text);

  // Loop through all options to find the longest
  // TODO can we use a glib function to get help text for the options?
  gsize longest_option = 0;
  for (int i = 0; i < CLI_MAX_OPTIONS; i++) {
    if (command->options[i].long_name == NULL)
      break;

    gchar* left_text = get_option_left_text(&command->options[i]);
    gsize  len       = strlen(left_text);
    g_free(left_text);
    if (len > longest_option)
      longest_option = len;
  }

  // Loop through again, printing each option this time
  for (int i = 0; i < CLI_MAX_OPTIONS; i++) {
    if (command->options[i].long_name == NULL)
      break;

    gchar* left_text = get_option_left_text(&command->options[i]);
    g_string_append(string, left_text);
    gsize len = strlen(left_text);
    g_free(left_text);

    gsize padding = 8 + longest_option - len;
    while (padding--)
      g_string_append_c(string, ' ');

    // TODO Line-wrapping
    gint left_index = 8 + longest_option;
    gint right_index =
        120;  // FIXME use get_columns(), but cache it outside the loop
    gint chars_per_line = right_index - left_index;
    g_string_append(string, command->options[i].description);
    g_string_append_c(string, '\n');
  }

  g_string_append(string, "\nFooter text here?\n");

  // Copy retult into heap memory so we can free the GString
  gchar* return_string = malloc(string->len);
  if (return_string == NULL) {
    g_printerr("Error assigning memory to get_help_from_portal's return value");
    return NULL;
  }
  memcpy(return_string, string->str, string->len);
  return return_string;
}

gint call_command_handler(Command* command, gint argc, gchar** argv)
{
  return command->handler(argc, argv);
}

gchar* get_command_name(Command* command)
{
  return command->name;
}
