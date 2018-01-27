#include <cstdlib>
#include <cassert>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "glibutil.h"
#include "settings.h"

boost::property_tree::ptree settings::pt;
std::string                 settings::storepath;

std::string                 settings::work_dir;
std::string                 settings::out_dir;

void settings::makedefaults() {
	if (out_dir.empty()) {
		/* on unix create remux.out directory in user home */
		/* on windows create remux in documents directory */
	}
	if (work_dir.empty()) {
		/* on unix create .remux.tmp directory in user home */
		/* on windows create remux in documents directory */
	}
}

void settings::init() {
#ifdef _WIN32
	storepath = util_filename_from_install_directory("config.xml");
#else
	storepath = "/etc/remux/config.xml";
#endif
	
	try {
		boost::property_tree::read_xml(storepath, pt);
	}
	catch (boost::property_tree::ptree_error e) {
		/*TODO print error e.what() */
	}
	
	out_dir = pt.get("outdirectory", "");
	work_dir = pt.get("workdirectory", "");
	
	makedefaults();
}

void settings::write() {
	
}

