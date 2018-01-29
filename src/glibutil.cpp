#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include "glibutil.h"

std::string util_build_filename(const std::string& first,
                                const std::string& second)
{
	std::string ret;
	
	gchar *builtstr = g_build_filename(first.c_str(), second.c_str(), NULL);
	ret = builtstr;
	g_free(builtstr);
	
	return ret;
}

#ifdef _WIN32
std::string util_install_directory(const std::string& second) {
	std::string instdir;
	gchar *install_directory;
	
	install_directory = g_win32_get_package_installation_directory_of_module(NULL);
	if (!install_directory)
		throw std::runtime_error("could not find base directory of installation"); /*TODO*/
	
	instdir = install_directory;
	g_free(install_directory);

	return instdir;
}
#endif

