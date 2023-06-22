#include "snake-internal.h"

#include "snake-property.h"
#include "snake_engine_v1.h"

#include <unistd.h>

#define MAP_SIZE 38

SnakeMap*    g_map          = NULL;
SnakeWindow* g_self_window  = NULL;
GtkWidget*   g_reset_button = NULL;
SnakeStatus  g_state        = SNAKE_STATUS_ALIVE;

static char  title_buffer[50];

gboolean
_snake_internal_gui_func (gpointer _self)
{
        SUnit* self = (SUnit*)_self;
        if (self == NULL)
                return 0;
        if (self->is_apple)
                gtk_image_set_from_resource (GTK_IMAGE (self->gui_object),
                                             SNAKE_IMAGE_APPLE);
        else if (self->is_snake_head) {
                switch (snake_get_orientation (g_map->snake)) {
                case SNAKE_ORIENTATION_DOWN:
                        gtk_image_set_from_resource (
                                GTK_IMAGE (self->gui_object),
                                SNAKE_IMAGE_HEAD_DOWN);
                        break;
                case SNAKE_ORIENTATION_LEFT:
                        gtk_image_set_from_resource (
                                GTK_IMAGE (self->gui_object),
                                SNAKE_IMAGE_HEAD_LEFT);
                        break;
                case SNAKE_ORIENTATION_RIGHT:
                        gtk_image_set_from_resource (
                                GTK_IMAGE (self->gui_object),
                                SNAKE_IMAGE_HEAD_RIGHT);
                        break;
                default:
                        gtk_image_set_from_resource (
                                GTK_IMAGE (self->gui_object),
                                SNAKE_IMAGE_HEAD_UP);
                        break;
                }
        } else if (self->is_snake_body)
                gtk_image_set_from_resource (GTK_IMAGE (self->gui_object),
                                             SNAKE_IMAGE_BODY);
        else
                gtk_image_set_from_resource (GTK_IMAGE (self->gui_object),
                                             SNAKE_IMAGE_NONE);
        return 0;
}

void
snake_internal_gui_func (SUnit* self)
{
        /*此处防止与 UI 部件起冲突*/
        g_idle_add (_snake_internal_gui_func, self);
}

void
snake_internal_init_gui (SnakeWindow* self)
{
        GtkWidget *main_grid, *image;

        /*获取主方格*/
        main_grid =
                GTK_WIDGET (gtk_widget_get_template_child (GTK_WIDGET (self),
                                                           SNAKE_TYPE_WINDOW,
                                                           "main_grid"));

        for (int y = 0; y < MAP_SIZE; ++y)
                for (int x = 0; x < MAP_SIZE; ++x) {
                        /*初始化：载入空白图形*/
                        image = gtk_image_new_from_resource (SNAKE_IMAGE_NONE);
                        /*将空白图形添加到 x y 处*/
                        gtk_grid_attach (GTK_GRID (main_grid),
                                         image,
                                         x,
                                         y,
                                         1,
                                         1);
                        /*为引擎地图 x y 处的方格设置 GUI 方法*/
                        s_unit_set_gui (OBJECT_CHK (g_map->_map, x, y),
                                        image,
                                        snake_internal_gui_func);
                }
}

/*键盘事件驱动函数*/
static void
snake_internal_keyboard_catch (GtkWidget* widget, gpointer data G_GNUC_UNUSED)
{
        guint     code;
        GdkEvent* ev = NULL;
        /*获取当前键盘事件类型*/
        ev = gtk_event_controller_get_current_event (
                GTK_EVENT_CONTROLLER (widget));
        if (ev == NULL) {
                g_print ("debug_assert1\n");
                return;
        }
        /*获取当前键盘按下的序号*/
        code = gdk_key_event_get_keycode (ev);
        switch (code) {
        case 113:
                snake_set_orientation (g_map->snake, SNAKE_ORIENTATION_LEFT);
                break;
        case 114:
                snake_set_orientation (g_map->snake, SNAKE_ORIENTATION_RIGHT);
                break;
        case 111:
                snake_set_orientation (g_map->snake, SNAKE_ORIENTATION_UP);
                break;
        case 116:
                snake_set_orientation (g_map->snake, SNAKE_ORIENTATION_DOWN);
                break;
        case 33:
        case 65: {
                /*游戏结束或产生错误后不能继续进行操作*/
                if (g_state != SNAKE_STATUS_DIE &&
                    g_state != SNAKE_STATUS_ERROR) {
                        g_state = (g_state == SNAKE_STATUS_SUSPEND
                                           ? SNAKE_STATUS_ALIVE
                                           : SNAKE_STATUS_SUSPEND);
                }
                break;
        }
        default:
                g_print ("未定义按键: %u\n", code);
                break;
        }
}

/**
 * 初始化键盘事件
 */
void
snake_internal_init_keyboard (SnakeWindow* self)
{
        GtkEventController* keyev;
        /*创建一个 GTK 事件驱动*/
        keyev = gtk_event_controller_key_new ();

        /*将窗口映射到 GTK 事件驱动*/
        gtk_widget_add_controller (GTK_WIDGET (self), keyev);

        /*将键盘按键按下的信号连接到事件驱动 keyev，并设置驱动函数*/
        g_signal_connect (keyev,
                          "key-pressed",
                          G_CALLBACK (snake_internal_keyboard_catch),
                          NULL);
}

gboolean
_snake_internal_update_window_state (gpointer self G_GNUC_UNUSED)
{
        static SnakeStatus old_state = SNAKE_STATUS_SUSPEND;
        SnakeRecUnit*      rec_unit;

        /*若当前状态与旧的状态一致，就不执行以下操作了*/
        if (old_state == g_state)
                return 0;

        switch (g_state) {
        case SNAKE_STATUS_ALIVE:
        case SNAKE_STATUS_APPLE:
                snprintf (title_buffer,
                          50,
                          "贪吃蛇-游戏中（长度: %u）",
                          g_map->snake->snake_length);
                gtk_window_set_title (GTK_WINDOW (g_self_window), title_buffer);
                gtk_widget_set_visible (g_reset_button, false);
                gtk_button_set_label (GTK_BUTTON (g_reset_button), "重开");
                break;
        /*游戏结束，向记录器记录游戏结果*/
        case SNAKE_STATUS_DIE:
                snprintf (title_buffer,
                          50,
                          "游戏结束！最终长度: %u",
                          g_map->snake->snake_length);
                rec_unit = snake_rec_unit_create (g_map->snake->snake_length,
                                                  (ulong)time (NULL));
                snake_recorder_add_rec_unit (global_recorder, rec_unit);
                gtk_window_set_title (GTK_WINDOW (g_self_window), title_buffer);
                gtk_widget_set_visible (g_reset_button, true);
                break;
        case SNAKE_STATUS_SUSPEND:
                snprintf (title_buffer,
                          50,
                          "贪吃蛇-暂停中（长度: %u）",
                          g_map->snake->snake_length);
                gtk_window_set_title (GTK_WINDOW (g_self_window), title_buffer);
                gtk_button_set_label (GTK_BUTTON (g_reset_button), "继续");
                gtk_widget_set_visible (g_reset_button, true);
                break;
        default:
                gtk_window_set_title (GTK_WINDOW (g_self_window),
                                      "贪吃蛇-程序错误？");
                break;
        }
        old_state = g_state;
        return 0;
}

void*
snake_internal_auto_snake (void* data G_GNUC_UNUSED)
{
        while (1) {
                while (g_state != SNAKE_STATUS_DIE &&
                       g_state != SNAKE_STATUS_ERROR &&
                       g_state != SNAKE_STATUS_SUSPEND) {
                        // sleep (1);
                        g_state = snake_set_next_orientation (
                                g_map->snake,
                                g_map->snake->now_orien);
                        if (g_state == SNAKE_STATUS_APPLE)
                                snake_map_set_apple (g_map);
                        g_idle_add (_snake_internal_update_window_state, NULL);
                        usleep (200000);
                        /*刷新标题*/
                        g_idle_add (_snake_internal_update_window_state, NULL);
                }
                usleep (100000);
                /*刷新标题*/
                g_idle_add (_snake_internal_update_window_state, NULL);
        }
}

void
snake_internal_init (SnakeWindow* self)
{
        pthread_t th;
        g_self_window = self;
        g_reset_button =
                GTK_WIDGET (gtk_widget_get_template_child (GTK_WIDGET (self),
                                                           SNAKE_TYPE_WINDOW,
                                                           "reset_button"));

        /*初始化引擎*/
        g_map = snake_map_new ();
        snake_map_alloc_map_with_xy (g_map, MAP_SIZE, MAP_SIZE);
        snake_map_related_uint (g_map);

        /*初始化地图部件*/
        snake_internal_init_gui (self);
        snake_map_init_snake (g_map);
        snake_internal_init_keyboard (self);

        /*释放一个连续移动蛇的线程*/
        pthread_create (&th, NULL, snake_internal_auto_snake, NULL);
        pthread_detach (th);
}

void
snake_internal_restart (GAction* action     G_GNUC_UNUSED,
                        GVariant* parameter G_GNUC_UNUSED,
                        SnakeWindow*        self)
{
        GtkGrid* grid;

        /*若当前为暂停状态，则此函数起取消暂停的作用*/
        if (g_state == SNAKE_STATUS_SUSPEND) {
                g_state = SNAKE_STATUS_ALIVE;
                return;
        }

        g_state = SNAKE_STATUS_DIE;
        grid    = GTK_GRID (gtk_widget_get_template_child (GTK_WIDGET (self),
                                                        SNAKE_TYPE_WINDOW,
                                                        "main_grid"));
        // g_print ("重开\n");
        snake_map_destory (g_map);
        for (int i = 0; i < 30; ++i)
                gtk_grid_remove_row (grid, 0);

        g_map = snake_map_new ();
        snake_map_alloc_map_with_xy (g_map, MAP_SIZE, MAP_SIZE);
        snake_map_related_uint (g_map);
        snake_internal_init_gui (self);
        snake_map_init_snake (g_map);
        g_state = SNAKE_STATUS_ALIVE;
}