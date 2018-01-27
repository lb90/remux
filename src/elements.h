#ifndef REMUX_ELEMENTS_H
#define REMUX_ELEMENTS_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

enum itemtype_t {
	itemtype_unknown = 0,
	itemtype_subtitle,
	itemtype_video,
	itemtype_audio,
};
enum codectype_t {
	codectype_unknown = 0,
	codectype_ac3,
	codectype_dts
};
struct item_t {
	explicit item_t();

	itemtype_t  type;

	std::string name;
	std::string lang;
	std::string uid;
	std::string num;
	int         tid;
	std::string codecname;
	codectype_t codectype;
	
	bool orig_forced;
	bool want_forced;
	bool orig_default;
	bool want_default;
};

struct err_t {
	explicit err_t();

	bool scan;
	std::string scan_description;
	bool conv;
	std::string conv_description;
	
	operator bool() const {
		return scan || conv;
	}
};
struct opt_t {
	explicit opt_t();

	bool convert_ac3ita_aac;
	bool leave_ac3;
	bool leave_dolby;
};
struct media_t {
	explicit media_t();
	
	bool isinit;
	bool isready;

	std::string name;
	std::string directory;
	std::string path;
	
	std::string outname;
	std::string outdirectory;
	std::string outpath;
	
	std::string title;
	
	opt_t       opt;
	std::vector<item_t> items;
	boost::property_tree::ptree pt;
	
	err_t       err;
};

#endif
