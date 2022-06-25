#pragma once

#include "command.h"

#include <glib.h>

extern Portal dynamic_launcher_portal;

gint dynamic_launcher_install(gint argc, gchar *argv[], gboolean show_help);
