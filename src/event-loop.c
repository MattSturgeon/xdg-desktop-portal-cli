#include "event-loop.h"

gboolean will_loop = FALSE;
static GMainLoop *loop = NULL;

gboolean get_loop_enabled()
{
    return will_loop;
}

void enable_loop()
{
    will_loop = TRUE;
}

void start_loop()
{
    if (will_loop)
    {
        loop = g_main_loop_new(NULL, FALSE);
        g_main_loop_run(loop);
    }
}

void finish_loop()
{
    if (loop) g_main_loop_quit(loop);
}
