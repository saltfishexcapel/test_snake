#include "snake-window.h"
#include "snake-config.h"

#include "snake-internal.h"

struct _SnakeWindow
{
        GtkApplicationWindow parent_instance;

        /*模板部件*/
        GtkHeaderBar* header_bar;
        GtkButton*    reset_button;
        GtkGrid*      main_grid;
};

G_DEFINE_TYPE (SnakeWindow, snake_window, GTK_TYPE_APPLICATION_WINDOW)

static void
snake_window_class_init (SnakeWindowClass* klass)
{
        GError*         err_ptr = NULL;
        GResource*      resource;
        GtkWidgetClass* widget_class = GTK_WIDGET_CLASS (klass);

        resource = g_resource_load (SNAKE_RESOURCE_PATH, &err_ptr);
        if (err_ptr != NULL) {
                fprintf (stderr, "读取 GResource 错误: %s\n", err_ptr->message);
                exit (1);
        }
        g_resources_register (resource);

        gtk_widget_class_set_template_from_resource (
                widget_class,
                "/org/example/snake/snake-interface.ui");
        gtk_widget_class_bind_template_child (widget_class,
                                              SnakeWindow,
                                              header_bar);
        gtk_widget_class_bind_template_child (widget_class,
                                              SnakeWindow,
                                              reset_button);
        gtk_widget_class_bind_template_child (widget_class,
                                              SnakeWindow,
                                              main_grid);
}

#define CUSTOM_MACRO_A(name, func)                 \
        g_autoptr (GSimpleAction) name##_action =  \
                g_simple_action_new (#name, NULL); \
        g_signal_connect (name##_action,           \
                          "activate",              \
                          G_CALLBACK ((func)),     \
                          self);                   \
        g_action_map_add_action (G_ACTION_MAP (self), G_ACTION (name##_action));

static void
snake_window_init (SnakeWindow* self)
{
        gtk_widget_init_template (GTK_WIDGET (self));
        snake_internal_init (self);

        CUSTOM_MACRO_A (game_restart, snake_internal_restart);
}
