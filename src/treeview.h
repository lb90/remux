#ifndef REMUX_TREEVIEW_H
#define REMUX_TREEVIEW_H

#include <cstdlib>
#include <vector>
#include <gtk/gtk.h>

enum treeview_select_t {
	select_all,
	select_none,
	select_invert
};

int treeview_init(GtkBuilder *builder);

void treeview_select(treeview_select_t sel);
void treeview_select_n(int n);

void treeview_getselection(std::vector<size_t>& indexv);

#endif
