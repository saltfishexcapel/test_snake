#include "snake-internal.h"

#include "snake_engine_v1.h"

#include <unistd.h>

SnakeMap*        g_map         = NULL;
SnakeWindow*     g_self_window = NULL;

SnakeOrientation g_orien       = SNAKE_ORIENTATION_UP;
SnakeStatus      g_state       = SNAKE_STATUS_ALIVE;

gboolean
_snake_internal_gui_func (gpointer _self)
{
        SUnit* self = (SUnit*)_self;
        if (self == NULL)
                return 0;
        if (self->is_apple)
                gtk_image_set_from_resource (GTK_IMAGE (self->gui_object),
                                             SNAKE_IMAGE_APPLE);
        else if (self->is_snake_head)
                gtk_image_set_from_resource (GTK_IMAGE (self->gui_object),
                                             SNAKE_IMAGE_HEAD);
        else if (self->is_snake_body)
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

        main_grid =
                GTK_WIDGET (gtk_widget_get_template_child (GTK_WIDGET (self),
                                                           SNAKE_TYPE_WINDOW,
                                                           "main_grid"));

        for (int y = 0; y < 30; ++y)
                for (int x = 0; x < 30; ++x) {
                        image = gtk_image_new_from_resource (SNAKE_IMAGE_NONE);
                        gtk_grid_attach (GTK_GRID (main_grid),
                                         image,
                                         x,
                                         y,
                                         1,
                                         1);
                        s_unit_set_gui (OBJECT_CHK (g_map->_map, x, y),
                                        image,
                                        snake_internal_gui_func);
                }
}

static void
snake_test (GtkWidget* widget, gpointer data G_GNUC_UNUSED)
{
        guint     code;
        GdkEvent* ev = NULL;
        ev           = gtk_event_controller_get_current_event (
                GTK_EVENT_CONTROLLER (widget));
        if (ev == NULL) {
                g_print ("debug_assert1\n");
                return;
        }
        code = gdk_key_event_get_keycode (ev);
        switch (code) {
        case 113:
                g_orien = SNAKE_ORIENTATION_LEFT;
                break;
        case 114:
                g_orien = SNAKE_ORIENTATION_RIGHT;
                break;
        case 111:
                g_orien = SNAKE_ORIENTATION_UP;
                break;
        case 116:
                g_orien = SNAKE_ORIENTATION_DOWN;
                break;
        case 33:
                g_state = (g_state == SNAKE_STATUS_ERROR ? SNAKE_STATUS_ALIVE
                                                         : SNAKE_STATUS_ERROR);
                g_print ("%s\n", (g_state == 3 ? "暂停" : "继续"));
                break;
        default:
                g_print ("key: %u\n", code);
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
        keyev = gtk_event_controller_key_new ();

        gtk_widget_add_controller (GTK_WIDGET (self), keyev);

        g_signal_connect (keyev, "key-pressed", G_CALLBACK (snake_test), NULL);
}

void*
snake_internal_auto_snake (void* data G_GNUC_UNUSED)
{
        while (1) {
                while (g_state != SNAKE_STATUS_DIE &&
                       g_state != SNAKE_STATUS_ERROR) {
                        // sleep (1);
                        g_state = snake_set_next_orientation (g_map->snake,
                                                              g_orien);
                        if (g_state == SNAKE_STATUS_APPLE)
                                snake_map_set_apple (g_map);

                        usleep (200000);
                }
                usleep (100000);
        }
}

void
snake_internal_init (SnakeWindow* self)
{
        pthread_t th;
        g_self_window = self;

        /*初始化引擎*/
        g_map = snake_map_new ();
        snake_map_alloc_map_with_xy (g_map, 30, 30);
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

        grid = GTK_GRID (gtk_widget_get_template_child (GTK_WIDGET (self),
                                                        SNAKE_TYPE_WINDOW,
                                                        "main_grid"));
        g_print ("重开\n");
        snake_map_destory (g_map);
        for (int i = 0; i < 30; ++i)
                gtk_grid_remove_row (grid, 0);

        g_map = snake_map_new ();
        snake_map_alloc_map_with_xy (g_map, 30, 30);
        snake_map_related_uint (g_map);
        snake_internal_init_gui (self);
        snake_map_init_snake (g_map);
        g_state = SNAKE_STATUS_ALIVE;
}