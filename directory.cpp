#include <cstdlib>
#include <cassert>
#include <string>
#include <gtk/gtk.h>
#include "model.h"

int scan_directory(char *dirpath) {
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
		model_add(TRUE, name, path.c_str());
		
		g_print("%s : %s\n", name, path.c_str());
		
		count++;
		if (count >= 2000)
			break;
	}
	
	g_dir_close(dir);
	dir = NULL;
	
	return 0;
}

