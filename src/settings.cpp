#include <cstdlib>
#include <cassert>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "glibutil.h"
#include "settings.h"

boost::property_tree::ptree settings::pt;

void settings::init() {
#ifdef _WIN32
	boost::property_tree::read_xml(util_filename_from_install_directory("config.xml"), tree);
#else
	boost::property_tree::read_xml("/etc/remux/config.xml", pt);
#endif
}
