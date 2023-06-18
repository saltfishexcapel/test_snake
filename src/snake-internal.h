#ifndef SNAKE_INTERNAL_H
#define SNAKE_INTERNAL_H

#include "snake-window.h"
#include "snake_engine_v1.h"

#pragma once

extern SnakeMap* g_map;

/*初始化*/
void snake_internal_init (SnakeWindow* self);

/*重开*/
extern void snake_internal_restart (GAction* action     G_GNUC_UNUSED,
                                    GVariant* parameter G_GNUC_UNUSED,
                                    SnakeWindow*        self);

#define SNAKE_IMAGE_NONE  "/org/example/snake/../data/picture/none.xpm"
#define SNAKE_IMAGE_APPLE "/org/example/snake/../data/picture/apple.xpm"
#define SNAKE_IMAGE_BODY  "/org/example/snake/../data/picture/snake_body.xpm"
#define SNAKE_IMAGE_HEAD_UP \
        "/org/example/snake/../data/picture/snake_head_up.xpm"
#define SNAKE_IMAGE_HEAD_DOWN \
        "/org/example/snake/../data/picture/snake_head_down.xpm"
#define SNAKE_IMAGE_HEAD_LEFT \
        "/org/example/snake/../data/picture/snake_head_left.xpm"
#define SNAKE_IMAGE_HEAD_RIGHT \
        "/org/example/snake/../data/picture/snake_head_right.xpm"

#endif