#ifndef REMUX_GLIBUTIL_H
#define REMUX_GLIBUTIL_H

#include <string>
#include <glib.h>

std::string util_build_filename(const std::string& first,
                                const std::string& second);

#ifdef _WIN32

std::string util_filename_from_install_directory(const std::string& second);

#endif

#endif

