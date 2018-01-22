#include <cstdlib>
#include <cassert>
#include <gtk/gtk.h>
#include <map>

extern GtkListStore *liststore;

int model_clear() {
	gtk_list_store_clear(liststore);
	
	return 0;
}

int model_add(const char *name,
              const char *path)
{
	assert(name && path);
	
	GtkTreeIter iter;
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter,
	                   0, (gboolean) TRUE,
	                   1, (gchar *) name,
	                   2, (gchar *) path,
	                   -1);
	
	return 0;
}

int model_remove(const std::vector<int>& indexv) {
	
}
