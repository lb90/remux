#include <cstdlib>
#include <cassert>
#include <gtk/gtk.h>
#include "gtkutil.h"

std::string util_build_filename(const std::string& first,
                                const std::string& second)
{
	std::string ret;
	
	gchar *builtstr = g_build_filename(first.c_str(), second.c_str(), NULL);
	ret = builtstr;
	g_free(builtstr);
	
	return ret;
}

