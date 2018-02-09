#ifndef REMUX_ELEMENTS_H
#define REMUX_ELEMENTS_H

#include <string>
#include <vector>
#include <map>
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
	codecid_eac3,
	codecid_dts,
	codecid_truehd,
	codecid_aac,
	codecid_mp2,
	codecid_mp3,
	codecid_vorbis,
	codecid_flac,
	codecid_pcm,
	codecid_other
};
struct coreitem_t {
	explicit coreitem_t();

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

class origitem_t
 : public coreitem_t
{
};

struct destitem_t
 : public coreitem_t
{
	explicit destitem_t(const origitem_t& orig);
	
	const origitem_t& orig;
	bool              want;
	
	std::string       outpath;
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

	std::vector<origitem_t>  origitems;
	std::vector<destitem_t>  destitems;
	boost::property_tree::ptree pt;
	
	err_t       err;
};

std::string codecid_to_name(codecid_t codecid);
codecid_t codecid_from_name(const std::string& name);

#endif
