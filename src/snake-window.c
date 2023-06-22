#include "snake-window.h"

#include "snake-config.h"
#include "snake-internal.h"
#include "snake-property.h"

struct _SnakeWindow
{
        GtkApplicationWindow parent_instance;

        /*模板部件*/
        GtkHeaderBar* header_bar;
        GtkButton*    reset_button;
        GtkFrame*     main_frame;
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
        gtk_widget_class_bind_template_child (widget_class,
                                              SnakeWindow,
                                              main_frame);
}

static void
snake_window_statistics_window (GSimpleAction* action G_GNUC_UNUSED,
                                GVariant* parameter   G_GNUC_UNUSED,
                                gpointer              user_data)
{
        static char   buffer[50];
        struct tm*    trans_tm;
        time_t        tmp_tm;
        SnakeRecUnit* rec_unit;
        GtkWindow*    self = GTK_WINDOW (user_data);
        GtkWidget *   stac_window, *scroll, *grid, *label;

        g_return_if_fail (GTK_IS_WINDOW (self));

        stac_window = gtk_window_new ();
        scroll      = gtk_scrolled_window_new ();
        grid        = gtk_grid_new ();

        gtk_window_set_title (GTK_WINDOW (stac_window), "得分排名");
        gtk_window_set_default_size (GTK_WINDOW (stac_window), 260, 300);

        snake_recorder_iter_reset (global_recorder);
        for (int i = 0; i < global_recorder->record_num; ++i) {
                rec_unit = snake_recorder_iter_get (global_recorder);
                tmp_tm   = (time_t)rec_unit->create_time;
                trans_tm = localtime (&tmp_tm);
                sprintf (buffer,
                         "时间: %d/%02d/%02d %02d:%02d:%02d, 得分: %u",
                         trans_tm->tm_year + 1900,
                         trans_tm->tm_mon + 1,
                         trans_tm->tm_mday,
                         trans_tm->tm_hour,
                         trans_tm->tm_min,
                         trans_tm->tm_sec,
                         rec_unit->snake_length);
                label = gtk_label_new (buffer);
                gtk_grid_attach (GTK_GRID (grid), label, 0, i, 1, 1);
        }

        gtk_window_set_child (GTK_WINDOW (stac_window), scroll);
        gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scroll), grid);

        gtk_window_present (GTK_WINDOW (stac_window));
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
        CUSTOM_MACRO_A (statistics, snake_window_statistics_window);
}
