#include "snake-application.h"
#include "snake-window.h"

struct _SnakeApplication
{
        GtkApplication parent_instance;
};

G_DEFINE_TYPE (SnakeApplication, snake_application, GTK_TYPE_APPLICATION)

SnakeApplication*
snake_application_new (gchar* application_id, GApplicationFlags flags)
{
        return g_object_new (SNAKE_TYPE_APPLICATION,
                             "application-id",
                             application_id,
                             "flags",
                             flags,
                             NULL);
}

static void
snake_application_finalize (GObject* object)
{
        SnakeApplication* self G_GNUC_UNUSED = (SnakeApplication*)object;
        G_OBJECT_CLASS (snake_application_parent_class)->finalize (object);
}

static void
snake_application_activate (GApplication* app)
{
        GtkWindow* window;
        g_assert (GTK_IS_APPLICATION (app));
        window = gtk_application_get_active_window (GTK_APPLICATION (app));
        if (window == NULL)
                window = g_object_new (SNAKE_TYPE_WINDOW,
                                       "application",
                                       app,
                                       NULL);

        gtk_window_present (window);
        
}

static void
snake_application_class_init (SnakeApplicationClass* klass)
{
        GObjectClass* object_class = G_OBJECT_CLASS (klass);
        GApplicationClass* app_class    = G_APPLICATION_CLASS (klass);

        object_class->finalize          = snake_application_finalize;
        app_class->activate             = snake_application_activate;
}

static void
snake_application_init (SnakeApplication* self)
{

}
