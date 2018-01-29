#include <cstdlib>
#include <cassert>
#include <locale>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "glibutil.h"
#include "app.h"
#include "settings.h"

boost::property_tree::ptree settings::pt;
std::string                 settings::storepath;

void settings::makedefaults() {
}

void settings::init() {
#ifdef _WIN32
	storepath = util_build_filename(app::install_dir, "config.xml");
#else
	storepath = "/etc/remux/config.xml";
#endif
	
	try {
		boost::property_tree::read_xml(storepath, pt, 0, std::locale("C"));
	}
	catch (boost::property_tree::ptree_error e) {
#ifndef _WIN32
		g_print("could not read config file. %s\n", e.what());
#endif
	}
}

void settings::write() {
	boost::property_tree::xml_writer_settings<std::string> xmlstyle(' ', 2);
	try {
		boost::property_tree::write_xml(storepath, pt, std::locale("C"), xmlstyle);
	}
	catch (boost::property_tree::ptree_error e) {
#ifndef _WIN32
		g_print("could not save config file. %s\n", e.what());
#endif
	}
}

