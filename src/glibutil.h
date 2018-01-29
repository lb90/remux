#ifndef REMUX_GLIBUTIL_H
#define REMUX_GLIBUTIL_H

#include <string>
#include <glib.h>

std::string util_build_filename(const std::string& first,
                                const std::string& second);

#ifdef _WIN32
std::string util_install_directory();
#endif

#endif

