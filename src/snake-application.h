#ifndef SNAKE_APPLICATION_H
#define SNAKE_APPLICATION_H

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SNAKE_TYPE_APPLICATION (snake_application_get_type ())

G_DECLARE_FINAL_TYPE (SnakeApplication,
                      snake_application,
                      SNAKE,
                      APPLICATION,
                      GtkApplication)

SnakeApplication* snake_application_new (gchar*            application_id,
                                         GApplicationFlags flags);

G_END_DECLS

#endif