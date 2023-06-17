#include "snake_engine_v1.h"

#include <stdlib.h>
#include <time.h>

void
s_unit_init (SUnit* sunit)
{
        if (sunit == NULL)
                return;
        sunit->prev = sunit->up = sunit->down = sunit->left = sunit->right =
                sunit->next                                 = NULL;
        sunit->is_apple                                     = false;
        sunit->is_snake_body                                = false;
        sunit->is_snake_head                                = false;
        sunit->gui_object                                   = NULL;
        sunit->gui_func                                     = NULL;
}

SUnit*
s_unit_new ()
{
        SUnit* s;
        s = OBJECT_NEW (SUnit);
        s_unit_init (s);
        return s;
}

void
s_unit_destory (SUnit* suint)
{
        if (suint != NULL)
                free (suint);
}

void
s_unit_set_gui (SUnit* self, void* gui_object, void (*gui_func) (SUnit*))
{
        if (self == NULL)
                return;
        self->gui_object = gui_object;
        self->gui_func   = gui_func;
}

void
snake_init (Snake* snake)
{
        if (snake == NULL)
                return;
        snake->snake_head = snake->snake_end = NULL;
        snake->snake_length                  = 0;
        snake->old_orien = snake->now_orien = SNAKE_ORIENTATION_UP;
}

Snake*
snake_new ()
{
        Snake* snake;
        snake = OBJECT_NEW (Snake);
        snake_init (snake);
        return snake;
}

void
snake_destory (Snake* snake)
{
        if (snake != NULL)
                free (snake);
}

void
snake_set_orientation (Snake* snake, SnakeOrientation orien)
{
        if (snake != NULL) {
                snake->old_orien = snake->now_orien;
                snake->now_orien = orien;
        }
}

SnakeOrientation
snake_get_orientation (Snake* snake)
{
        if (snake != NULL)
                return snake->now_orien;
        return SNAKE_ORIENTATION_UP;
}

void
snake_eat_apple (Snake* snake, SUnit* apple)
{
        if (snake == NULL || apple == NULL)
                return;
        apple->is_apple = false;
        snake->snake_length += 1;
        /*将当前苹果位置设为蛇身*/
        apple->is_snake_body             = true;
        apple->is_snake_head             = true;
        snake->snake_head->is_snake_head = false;
        /*双向绑定*/
        snake->snake_head->prev = apple;
        apple->next             = snake->snake_head;
        /*更新蛇头*/
        snake->snake_head = apple;
        GUI_FUNC_RUN (snake->snake_head);
        GUI_FUNC_RUN (snake->snake_head->next);
}

void
snake_move (Snake* snake, SUnit* next_s_unit)
{
        SUnit* end_node;
        if (snake == NULL || next_s_unit == NULL)
                return;
        /*缩短蛇尾节点*/
        end_node         = snake->snake_end;
        snake->snake_end = end_node->prev;
        /*尾节点脱节*/
        snake->snake_end->next = NULL;
        end_node->prev = end_node->next = NULL;
        end_node->is_snake_body         = false;
        /*延伸头节点，双向关联*/
        snake->snake_head->prev          = next_s_unit;
        next_s_unit->next                = snake->snake_head;
        snake->snake_head->is_snake_head = false;
        /*更新头节点*/
        snake->snake_head                = next_s_unit;
        snake->snake_head->is_snake_body = true;
        snake->snake_head->is_snake_head = true;
        GUI_FUNC_RUN (snake->snake_head);
        GUI_FUNC_RUN (snake->snake_head->next);
        GUI_FUNC_RUN (end_node);
}

SnakeStatus
snake_set_next_orientation (Snake* snake, SnakeOrientation orien)
{
        SUnit* next_s_unit;

        if (snake == NULL)
                return SNAKE_STATUS_ERROR;

        /*掉头不会倒转方向，不会死*/
        if (((snake->now_orien + snake->old_orien) == 1) ||
            ((snake->now_orien + snake->old_orien) == 5)) {
                orien = snake->old_orien;
                snake->now_orien = snake->old_orien;
        }

        /*先判断下个方向的情况：无情况或蛇身或苹果*/
        switch (orien) {
        case SNAKE_ORIENTATION_DOWN:
                next_s_unit = snake->snake_head->down;
                break;

        case SNAKE_ORIENTATION_LEFT:
                next_s_unit = snake->snake_head->left;
                break;

        case SNAKE_ORIENTATION_RIGHT:
                next_s_unit = snake->snake_head->right;
                break;

        case SNAKE_ORIENTATION_UP:
                next_s_unit = snake->snake_head->up;
                break;

        default:
                return SNAKE_STATUS_ERROR;
        }

        /*碰到墙死*/
        if (next_s_unit == NULL)
                return SNAKE_STATUS_DIE;
        /*是蛇身*/
        if (next_s_unit->is_snake_body && next_s_unit != snake->snake_end) {
                snake_move (snake, next_s_unit);
                return SNAKE_STATUS_DIE;
                /*是苹果*/
        } else if (next_s_unit->is_apple) {
                snake_eat_apple (snake, next_s_unit);
                return SNAKE_STATUS_APPLE;
        } else
                snake_move (snake, next_s_unit);

        return SNAKE_STATUS_ALIVE;
}

void
snake_map_init (SnakeMap* smp)
{
        if (smp == NULL)
                return;
        smp->_map          = NULL;
        smp->snake         = NULL;
        smp->is_game_over  = false;
        smp->is_game_ready = false;
        smp->_x            = 0;
        smp->_y            = 0;
}

SnakeMap*
snake_map_new ()
{
        SnakeMap* smp;
        smp = OBJECT_NEW (SnakeMap);
        snake_map_init (smp);
        return smp;
}

void
snake_map_unset (SnakeMap* smp)
{
        if (smp == NULL)
                return;
        for (int y = 0; y < smp->_y; ++y) {
                for (int x = 0; x < smp->_x; ++x)
                        s_unit_destory (OBJECT_CHK (smp->_map, x, y));
                free (smp->_map[y]);
        }
        free (smp->_map);
        if (smp->snake)
                snake_destory (smp->snake);
}

void
snake_map_destory (SnakeMap* smp)
{
        if (smp == NULL)
                return;
        snake_map_unset (smp);
        free (smp);
}

void
snake_map_alloc_map_with_xy (SnakeMap* smp, uint _x, uint _y)
{
        if (smp == NULL)
                return;
        smp->_x   = _x;
        smp->_y   = _y;
        smp->_map = (_s_unit_ptr**)malloc (sizeof (_s_unit_ptr*) * _y);
        for (int y = 0; y < _y; ++y) {
                smp->_map[y] = (_s_unit_ptr*)malloc (sizeof (_s_unit_ptr) * _x);
                for (int x = 0; x < _x; ++x)
                        OBJECT_CHK (smp->_map, x, y) = s_unit_new ();
        }
}

void
snake_map_related_uint (SnakeMap* smp)
{
        if (smp == NULL)
                return;

        /*左右绑定*/
        for (int y = 0; y < smp->_y; ++y) {
                for (int x = 0; x < smp->_x - 1; ++x) {
                        OBJECT_CHK (smp->_map, x, y)->right =
                                OBJECT_CHK (smp->_map, x + 1, y);
                        OBJECT_CHK (smp->_map, x + 1, y)->left =
                                OBJECT_CHK (smp->_map, x, y);
                }
                /*地图左右联通
                OBJECT_CHK (smp->_map, smp->_x - 1, y)->right =
                        OBJECT_CHK (smp->_map, 0, y);
                OBJECT_CHK (smp->_map, 0, y)->left =
                        OBJECT_CHK (smp->_map, smp->_x - 1, y);*/
        }
        /*上下绑定*/
        for (int x = 0; x < smp->_x; ++x) {
                for (int y = 0; y < smp->_y - 1; ++y) {
                        OBJECT_CHK (smp->_map, x, y)->down =
                                OBJECT_CHK (smp->_map, x, y + 1);
                        OBJECT_CHK (smp->_map, x, y + 1)->up =
                                OBJECT_CHK (smp->_map, x, y);
                }
                /*地图上下联通
                OBJECT_CHK (smp->_map, x, smp->_y - 1)->down =
                        OBJECT_CHK (smp->_map, x, 0);
                OBJECT_CHK (smp->_map, x, 0)->up =
                        OBJECT_CHK (smp->_map, x, smp->_y - 1);*/
        }
        /*当地图关联好后即可准备开始初始化蛇*/
        smp->is_game_ready = true;
}

void
snake_map_set_apple (SnakeMap* smp)
{
        uint set_x, set_y;

        if (smp == NULL)
                return;
        srand ((unsigned int)time (NULL));
        do {
                set_x = rand () % smp->_x;
                set_y = rand () % smp->_y;
        } while (OBJECT_CHK (smp->_map, set_x, set_y)->is_snake_body);
        OBJECT_CHK (smp->_map, set_x, set_y)->is_apple = true;
        GUI_FUNC_RUN (OBJECT_CHK (smp->_map, set_x, set_y));
}

void
snake_map_init_snake (SnakeMap* smp)
{
        uint   head_x, head_y;
        Snake* snake;
        SUnit* next;
        if (smp == NULL || !smp->is_game_ready)
                return;
        /*指定地图中心为蛇头位置*/
        head_x = smp->_x / 2;
        head_y = smp->_y / 2;
        snake = smp->snake = snake_new ();

        /*设置初始蛇*/
        snake->snake_length                                   = 2;
        OBJECT_CHK (smp->_map, head_x, head_y)->is_snake_body = true;
        OBJECT_CHK (smp->_map, head_x, head_y)->is_snake_head = true;
        snake->snake_head   = OBJECT_CHK (smp->_map, head_x, head_y);
        next                = snake->snake_head->down;
        next->is_snake_body = true;
        snake->snake_end    = next;
        /*关联初始两个节点*/
        snake->snake_head->next = next;
        next->prev              = snake->snake_head;
        GUI_FUNC_RUN (snake->snake_head);
        GUI_FUNC_RUN (snake->snake_end);
        /*设置初始苹果*/
        snake_map_set_apple (smp);
}