#include <cstdlib>
#include <cassert>
#include <gtk/gtk.h>
#include "util.h"
#include "treeview.h"
#include "signalcentre.h"

GtkListStore *liststore;

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

void cb_media_toggled(GtkCellRendererToggle *cell,
                      gchar *path_string, gpointer) {
	GtkTreeIter iter;
	if (gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(liststore),
	                                        &iter, path_string))
	{
		gboolean booleanval;
		gtk_tree_model_get(GTK_TREE_MODEL(liststore),
		                   &iter, 0, &booleanval, -1);
		booleanval = !booleanval;
		gtk_list_store_set(liststore,
		                   &iter, 0, booleanval, -1);
	}
}

int treeview_init_store(GtkWidget *treeview) {
	liststore = gtk_list_store_new(3, G_TYPE_BOOLEAN,
	                                  G_TYPE_STRING,
	                                  G_TYPE_STRING);
	assert(liststore);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(liststore));
	g_object_unref(liststore);
	
	return 0;
}

int treeview_init_columns(GtkWidget *treeview) {
	GtkTreeViewColumn *col;
	GtkCellRenderer   *renderer;

	/* --- Column #1 --- */
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "");
	gtk_tree_view_column_set_clickable(col, TRUE);

	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

	renderer = gtk_cell_renderer_toggle_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(renderer),
	                                   FALSE);
	gtk_cell_renderer_toggle_set_activatable(GTK_CELL_RENDERER_TOGGLE(renderer),
	                                         TRUE);
	g_signal_connect(renderer, "toggled", G_CALLBACK(cb_media_toggled), NULL);
	gtk_tree_view_column_add_attribute(col, renderer, "active", 0);

	/* --- Column #2 --- */
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Media");
	gtk_tree_view_column_set_clickable(col, TRUE);

	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", 1);
	
	return 0;
}

int treeview_init(GtkBuilder *builder) {
	GtkWidget *treeview;
	treeview = GTK_WIDGET(gtk_builder_get_object(builder, "treeview"));
	assert(treeview);
	
	treeview_init_store(treeview);
	
	gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(treeview), TRUE);
	
	treeview_init_columns(treeview);

	return 0;
}
