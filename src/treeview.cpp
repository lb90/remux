#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <gtk/gtk.h>
#include "util.h"
#include "treeview.h"
#include "signalcentre.h"

GtkListStore *liststore;
GtkWidget    *treeview;

HANDLER_BEGIN
void cb_row_activated(GtkTreeView* treeview,
                      GtkTreePath *path,
                      GtkTreeViewColumn,
                      gpointer)
{
	gint *indices, n;
	assert(gtk_tree_path_get_depth(path) == 1);
	indices = gtk_tree_path_get_indices(path);
	n = indices[0];
	signalcentre::emit("treeview\\rowactivated", n);
}
HANDLER_END

int treeview_init_store(GtkWidget *treeview) {
	liststore = gtk_list_store_new(2, G_TYPE_STRING,
	                                  G_TYPE_STRING);
	assert(liststore);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(liststore));
	g_object_unref(liststore);
	
	return 0;
}

int treeview_init_columns(GtkBuilder *builder, GtkWidget *treeview) {
	GtkTreeViewColumn *col;
	GtkCellRenderer   *ren;
	std::vector<std::pair<std::string, int>> colv = {
		std::make_pair("name",      0),
		std::make_pair("directory", 1)
	};
	std::string colstring = "col_";
	std::string renstring = "ren_";
	for (const auto& c : colv) {
		col = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object( builder, (colstring + c.first).c_str() ));
		ren = GTK_CELL_RENDERER(gtk_builder_get_object( builder, (renstring + c.first).c_str() ));
		gtk_tree_view_column_add_attribute(col, ren, "text", c.second);
	}
	return 0;
}

int treeview_init(GtkBuilder *builder) {
	treeview = GTK_WIDGET(gtk_builder_get_object(builder, "treeview"));
	
	treeview_init_store(treeview);
	treeview_init_columns(builder, treeview);

	return 0;
}

void treeview_select(treeview_select_t sel) {
	switch (sel) {
		case select_all:
			gtk_tree_selection_select_all(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)));
			break;
		case select_none:
			gtk_tree_selection_unselect_all(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)));
			break;
		case select_invert:
			break;
	}
}

