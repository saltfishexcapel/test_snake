#ifndef SNAKE_ENGINE_V1_H
#define SNAKE_ENGINE_V1_H

#include "object-type.h"

#define OBJECT_CHK(obj2, x, y) (obj2)[y][x]

/*贪吃蛇单方格对象*/
OBJECT_SET_TYPE (SUnit, s_unit)
/*贪吃蛇本蛇*/
OBJECT_SET_TYPE (Snake, snake)
/*贪吃蛇地图对象*/
OBJECT_SET_TYPE (SnakeMap, snake_map)

typedef SUnit*                 _s_unit_ptr;
typedef enum _SnakeOrientation SnakeOrientation;
typedef enum _SnakeStatus      SnakeStatus;

enum _SnakeOrientation
{
        SNAKE_ORIENTATION_LEFT,
        SNAKE_ORIENTATION_RIGHT,
        SNAKE_ORIENTATION_UP,
        SNAKE_ORIENTATION_DOWN,
};

enum _SnakeStatus
{
        SNAKE_STATUS_ALIVE,
        SNAKE_STATUS_DIE,
        SNAKE_STATUS_APPLE,
        SNAKE_STATUS_ERROR,
};

struct _SUnitClass
{
        /*方格上下左右的对象*/
        SUnit* up;
        SUnit* down;
        SUnit* left;
        SUnit* right;

        /*蛇的下一个节点*/
        SUnit* next;
        SUnit* prev;

        /*方格属性*/
        bool is_snake_head;
        bool is_snake_body;
        bool is_apple;

        /*GUI 对象*/
        void* gui_object;
        void (*gui_func) (SUnit*);
};

void s_unit_set_gui (SUnit* self, void* gui_object, void (*gui_func) (SUnit*));

#define GUI_FUNC_RUN(sunit)                       \
        if (sunit->gui_object && sunit->gui_func) \
                sunit->gui_func (sunit);

struct _SnakeClass
{
        SUnit* snake_head;
        SUnit* snake_end;
        uint   snake_length;
};

/*吃掉苹果，将苹果状态消除*/
void snake_eat_apple (Snake* snake, SUnit* apple);

/*蛇移动到下一格*/
void snake_move (Snake* snake, SUnit* next_s_unit);

/*控制蛇向下一个方向移动一格*/
SnakeStatus snake_set_next_orientation (Snake* snake, SnakeOrientation orien);

struct _SnakeMapClass
{
        /*地图*/
        _s_unit_ptr** _map;

        /*地图属性*/
        uint   _x;
        uint   _y;

        Snake* snake;
        bool   is_game_over;
        bool   is_game_ready;
};

/*根据 _x _y 分配地图*/
void snake_map_alloc_map_with_xy (SnakeMap* smp, uint _x, uint _y);

/*关联地图中所有方格*/
void snake_map_related_uint (SnakeMap* smp);

/*在地图上随机放置一个苹果*/
void snake_map_set_apple (SnakeMap* smp);

/*初始化蛇*/
void snake_map_init_snake (SnakeMap* smp);

#endif