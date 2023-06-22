#include "snake-property.h"

#include <stdio.h>

int
main (int argc, char** argv)
{
        SnakeProperty* prty;
        SnakePrtyFile* prtyf;

        prty = snake_property_new ();
        snake_property_set_app_path (prty, ".config/test_snake");
        snake_property_load_prty (prty);

        prtyf = snake_property_add_prty_file (prty, "teset_snake.conf");                

        snake_property_destory (prty);

        return 0;
}