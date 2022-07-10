#include "xdg-portal.h"

#include <glib.h>

static XdpPortal* xdg_portal = NULL;

void init_xdg_portal()
{
  if (xdg_portal)
    return;

  xdg_portal = xdp_portal_new();
}

XdpPortal* get_xdg_portal()
{
  return xdg_portal;
}
