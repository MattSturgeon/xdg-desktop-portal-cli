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
        "The DynamicLauncher portal allows sandboxed (or unsandboxed) "
        "applications to install launchers (.desktop files) which have an icon "
        "associated with them and which execute a command in the application. "
        "The desktop environment would display the launcher to the user in its "
        "menu of installed applications. For example this can be used by a "
        "sandboxed browser to install web app launchers. The portal also "
        "allows apps to uninstall the launchers, launch them, and read the "
        "desktop file and icon data for them.",
    .commands =
        {
            &install_command,
            &uninstall_command,
            &get_command,
            &get_icon_command,
            &launch_command,
        },
};
