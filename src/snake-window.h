#ifndef SNAKE_WINDOW_H
#define SNAKE_WINDOW_H

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SNAKE_TYPE_WINDOW (snake_window_get_type ())

G_DECLARE_FINAL_TYPE (SnakeWindow,
                      snake_window,
                      SNAKE,
                      WINDOW,
                      GtkApplicationWindow)


G_END_DECLS

#endif