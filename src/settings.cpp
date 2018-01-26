#include <cstdlib>
#include <cassert>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "glibutil.h"
#include "settings.h"

boost::property_tree::ptree settings::pt;

void settings::init() {
	#ifdef _WIN32
	file = util_filename_from_install_directory("config.xml");
	#else
	file = "/etc/remux/config.xml";
	#endif
	
	try {
		boost::property_tree::read_xml(file, pt);
	}
	catch (boost::property_tree::ptree_error e) {
		/*TODO print error e.what() */
	}
}

void settings::write() {
	
}

