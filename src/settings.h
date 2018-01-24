#ifndef REMUX_SETTINGS_H
#define REMUX_SETTINGS_H

#include <boost/property_tree/ptree.hpp>

class settings {
public:
	static
	boost::property_tree::ptree pt;
	
	void init();
};

#endif
