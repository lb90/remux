#include <cstdlib>
#include <cassert>
#include <vector>
#include <climits>
#include <gtk/gtk.h>
#include "glibutil.h"
#include "model.h"
#include "elements.h"
#include "settings.h"

extern GtkListStore *liststore;

std::vector<media_t> elementv;

int model_clear() {
	gtk_list_store_clear(liststore);
	elementv.clear();
	
	return 0;
}

int model_add(const char *name,
              const char *directory)
{
	assert(name && directory);
	
	GtkTreeIter iter;
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter,
	                   0, (gchar *) "",//"view-refresh",
	                   1, (gchar *) name,
	                   2, (gchar *) directory,
	                   -1);

	elementv.emplace_back();
	media_t& elem = elementv.back();

	elem.name = name;
	elem.directory = directory;
	elem.path = util_build_filename(elem.directory, elem.name);

	elem.outname = elem.name;
	elem.outdirectory = util_build_filename(elem.directory, "Remux");
	elem.outpath = util_build_filename(elem.outdirectory, elem.outname);

	return 0;
}

void model_remove(size_t i) {
	GtkTreeIter iter;
	gint n;
	
	elementv.erase(elementv.begin() + i);
	
	assert(i < INT_MAX);
	n = int(i);
	
	gboolean ret = FALSE;
	ret = gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(liststore), &iter, NULL, n);
	assert(ret);
	gtk_list_store_remove(liststore, &iter);
}

void model_enqueue(size_t i) {
	GtkTreeIter iter;
	gint n;

	assert(i < elementv.size());

	elementv[i].isready = true;
	
	assert(i < INT_MAX);
	n = int(i);
	gboolean ret = FALSE;
	ret = gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(liststore), &iter, NULL, n);
	assert(ret);
	gtk_list_store_set(liststore, &iter, 0, "dialog-ok", -1);
}

