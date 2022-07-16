#include "dynamic-launcher.h"

// Commands defined in the respective files in src/dynamic-launcher.
// Declared here for use by dynamic_launcher_portal
extern Command install_command;
extern Command uninstall_command;
extern Command get_command;
extern Command get_icon_command;
extern Command launch_command;

Portal dynamic_launcher_portal = {
    .name    = "dynamic-launcher",
    .aliases = {"launcher", "desktop-entry"},
    .help_text =
        "TODO: Copy paste from docs... describe what the dynamic-launcher "
        "portal does...",
    .commands =
        {
            &install_command,
            &uninstall_command,
            &get_command,
            &get_icon_command,
            &launch_command,
        },
};
