#ifndef REMUX_ELEMENTS_H
#define REMUX_ELEMENTS_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

enum itemtype_t {
	ITEMTYPE_UNKNOWN = 0,
	ITEMTYPE_SUBTITLE,
	ITEMTYPE_VIDEO,
	ITEMTYPE_AUDIO,
};
struct item_t {
	itemtype_t  itemtype;

	std::string name;
	std::string language;
	std::string uid;
	
	std::string codec;
	
	bool orig_forced;
	bool want_forced;
	bool orig_default;
	bool want_deafult;
};

struct errors_t {
	bool error;
	std::string errorstring;
	bool converterror;
	std::string converterrorstring;
};
struct options_t {
	bool ac3ita_aac;
	bool intact_dts;
	bool intact_ac3;
};
struct media_t {
	std::string name;
	std::string directory;
	std::string path;
	
	std::string outdirectory;
	std::string outname;
	
	errors_t  errors;
	options_t options;
	
	std::vector<item_t> itemv;
	
	boost::property_tree::ptree pt;
};

#endif
