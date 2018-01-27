#ifndef REMUX_SETTINGS_H
#define REMUX_SETTINGS_H

#include <boost/property_tree/ptree.hpp>

#include <string>

class settings {
public:
	static
	boost::property_tree::ptree pt;
	static std::string storepath;
	
	static void init();
	static void write();

private:
	void makedefaults();
};

#endif
