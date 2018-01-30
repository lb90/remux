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
	signalcentre::emit("element\\activate", n);
}
HANDLER_END

int treeview_init_store(GtkWidget *treeview) {
	liststore = gtk_list_store_new(3, G_TYPE_STRING,
	                                  G_TYPE_STRING,
	                                  G_TYPE_STRING);
	assert(liststore);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(liststore));
	g_object_unref(liststore);
	
	return 0;
}

int treeview_init_columns(GtkBuilder *builder, GtkWidget *treeview) {
	GtkTreeViewColumn *col;
	GtkCellRenderer   *ren;
	
	col = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "col_icon"));
	ren = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "ren_icon"));
	gtk_tree_view_column_add_attribute(col, ren, "icon-name", 0);
	
	std::vector<std::pair<std::string, int>> colv = {
		std::make_pair("name",      1),
		std::make_pair("directory", 2)
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

void treeview_select_n(int n) {
	GtkTreePath *path;
	path = gtk_tree_path_new_from_indices(n, -1);
	
	treeview_select(select_none);
	gtk_tree_selection_select_path(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)), path);
	
	gtk_tree_path_free(path);
}

void treeview_getselection(std::vector<size_t>& indexv) {
	indexv.clear();

	GList *list, *auxlist;
	GtkTreeModel *treemodel = GTK_TREE_MODEL(liststore);
	list = gtk_tree_selection_get_selected_rows(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)),
	                                            &treemodel);

	if (list) {
		auxlist = list;
		
		for (;;auxlist = auxlist->next) {
			GtkTreePath *path;
			gint        *indices, depth, n;
			
			if (auxlist == NULL) break;
			
			path = (GtkTreePath*) auxlist->data;
			if (path == NULL) break;
			
			indices = gtk_tree_path_get_indices_with_depth(path, &depth);
			assert(depth == 1);
			
			n = indices[0];
			assert(n >= 0);
			indexv.push_back(size_t(n));
		}

		g_list_free_full(list, (GDestroyNotify) gtk_tree_path_free);
	}
}

