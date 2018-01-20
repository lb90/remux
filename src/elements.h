#ifndef REMUX_ELEMENTS_H
#define REMUX_ELEMENTS_H

#include <string>
#include <vector>
#include <memory>
#include <boost/property_tree/ptree.hpp>

enum itemtype_t {
	ITEMTYPE_UNKNOWN = 0,
	ITEMTYPE_TRACK,
	ITEMTYPE_SUBTITLE
};

struct item_t {
	itemtype_t  itemtype;

	std::string name;
	std::string language;
	std::string uid;
	
	bool forced;
	bool defaul;
};

enum tracktype_t {
	TRACKTYPE_UNKNOWN = 0,
	TRACKTYPE_VIDEO,
	TRACKTYPE_AUDIO
};
struct track_t
 : public item_t
{
	tracktype_t tracktype;
	std::string codec;
};

struct subtitle_t
 : public item_t
{
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
	
	std::vector<std::unique_ptr<item_t>>
	
	boost::property_tree::ptree pt;
};

#endif
