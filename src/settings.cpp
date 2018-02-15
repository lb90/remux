#include <cstdlib>
#include <cassert>
#include <locale>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "glibutil.h"
#include "app.h"
#include "settings.h"
#ifdef _WIN32
#include "win.h"
#endif

boost::property_tree::ptree settings::pt;
std::string                 settings::storepath;

void settings::makedefaults() {
}

void settings::init() {
#ifdef _WIN32
	std::string configfolder;
	if (get_config_folder(configfolder) == 0)
		storepath = util_build_filename(configfolder, "config.xml");
	else storepath = "config.xml";
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
	boost::property_tree::ptree newpt;

	newpt.put("dir.mkvtoolnix", app::mkvtoolnix_dir);
	newpt.put("dir.ffmpeg", app::ffmpeg_dir);
	
	boost::property_tree::ptree tagpt;
	for (const std::string& tag : app::subtitletags) {
		tagpt.add("tag", tag);
	}

	newpt.add_child("tags.subtitle", tagpt);
	
	newpt.put("showwindow", app::showwindow);

	boost::property_tree::xml_writer_settings<std::string> xmlstyle(' ', 2);
	try {
		boost::property_tree::write_xml(storepath, newpt, std::locale("C"), xmlstyle);
	}
	catch (boost::property_tree::ptree_error e) {
#ifndef _WIN32
		g_print("could not save config file. %s\n", e.what());
#endif
	}
	
	
}

