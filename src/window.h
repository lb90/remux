#ifndef REMUX_WINDOW_H
#define REMUX_WINDOW_H

#include <gtk/gtk.h>

int window_init(GtkWidget *window, GtkApplication *gtkapp);
GtkWindow* get_window(GtkWindow *window);

#endif
