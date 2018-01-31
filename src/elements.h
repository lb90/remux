#ifndef REMUX_ELEMENTS_H
#define REMUX_ELEMENTS_H

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

enum itemtype_t {
	itemtype_unknown = 0,
	itemtype_video,
	itemtype_audio,
	itemtype_subtitle,
	itemtype_button
};
enum codecid_t {
	codecid_unknown = 0,
	codecid_ac3,
	codecid_dolby,
	codecid_aac
};
struct item_t {
	explicit item_t();
	explicit item_t(const item_t&);

	itemtype_t  type;

	std::string name;
	std::string lang;
	std::string langid;
	std::string uid;
	std::string num;
	int         tid;
	std::string codecname;
	codecid_t   codecid;
	
	bool isforced;
	bool isdefault;
};

struct newitem_t
 : public item_t
{
	explicit newitem_t(const item_t& backitem,
	                   size_t backn);
	
	const item_t& backitem;
	size_t        backn;
	bool          want;
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

	bool ok_convert_ac3ita_aac;
	bool want_convert_ac3ita_aac;
	bool want_keep_ac3;
	bool want_keep_dolby;
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
	std::vector<item_t>     items;
	std::vector<newitems_t> newitems;
	boost::property_tree::ptree pt;
	
	err_t       err;
};

#endif
