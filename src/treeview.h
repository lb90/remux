#ifndef REMUX_TREEVIEW_H
#define REMUX_TREEVIEW_H

#include <gtk/gtk.h>

enum treeview_select_t {
	select_all,
	select_none,
	select_invert
};

int treeview_init(GtkBuilder *builder);

void treeview_select(treeview_select_t sel);

#endif
