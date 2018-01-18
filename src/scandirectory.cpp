#include <cstdlib>
#include <cassert>
#include <string>
#include <gtk/gtk.h>
#include "model.h"
#include "scandirectory.h"

int scan_directory(const char *dirpath) {
	GDir *dir;
	int count;
	
	dir = g_dir_open(dirpath, 0, NULL);
	if (!dir)
		return -1;

	count = 0;
	while (const gchar *name = g_dir_read_name(dir)) {
		std::string path;
		path = dirpath;
		path += "/";
		path += name;
		if (g_file_test(path.c_str(), G_FILE_TEST_IS_DIR))
			continue;
		model_add(TRUE, name, path.c_str());
		
		count++;
		if (count >= 2000)
			break;
	}
	
	g_dir_close(dir);
	dir = NULL;
	
	return 0;
}

