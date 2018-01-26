#include <cstdlib>
#include <cassert>
#include <vector>
#include <gtk/gtk.h>
#include "glibutil.h"
#include "model.h"
#include "elements.h"

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
	                   0, (gchar *) name,
	                   1, (gchar *) directory,
	                   -1);

	elementv.emplace_back();
	media_t& e = elementv.back();
	e.name = name;
	e.directory = directory;
	e.path = util_build_filename(e.directory, e.path);
	
	
	return 0;
}

int model_remove(const std::vector<int>& indexv) {
	return 0;
}
