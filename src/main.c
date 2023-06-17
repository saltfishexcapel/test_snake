#include "snake-internal.h"
#include "snake_engine_v1.h"
#include "snake-application.h"

int
main (int argc, char* argv[])
{
        int               ret;
        SnakeApplication* app;

        app = snake_application_new ("org.example.snake",
                                     G_APPLICATION_FLAGS_NONE);
        ret = g_application_run (G_APPLICATION (app), argc, argv);

        snake_map_destory (g_map);

        return ret;
}