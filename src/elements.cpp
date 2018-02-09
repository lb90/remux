#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include "model.h"
#include "elements.h"

std::map<codecid_t, std::string> codecid_name = {
	{codecid_ac3,    "AC-3"},
	{codecid_eac3,   "EAC-3"},
	{codecid_dts,    "DTS"},
	{codecid_truehd, "TrueHD"},
	{codecid_aac,    "AAC"},
	{codecid_mp2,    "MP2"},
	{codecid_mp3,    "MP3"},
	{codecid_vorbis, "Vorbis"},
	{codecid_flac,   "FLAC"},
	{codecid_pcm,    "PCM"},
};

media_t::media_t()
 : isinit(false),
   isready(false),
   name(),
   directory(),
   path(),
   outname(),
   outdirectory(),
   outpath(),
   title(),
   origitems(),
   destitems(),
   pt(),
   err()
 {}

err_t::err_t()
 : scan(false),
   scan_description(),
   conv(false),
   conv_description()
 {}

coreitem_t::coreitem_t()
 : type(itemtype_unknown),
   name(),
   lang(),
   langid(),
   uid(),
   num(),
   tid(-1),
   codecname(),
   codecid(codecid_unknown),
   isforced(false),
   isdefault(false)
 {}

destitem_t::destitem_t(const origitem_t& orig)
 : coreitem_t(orig),
   orig(orig),
   want(true),
   outpath()
 {}

std::string codecid_to_name(codecid_t codecid) {
	if (codecid == codecid_unknown) return "";
	if (codecid == codecid_other) return "";
	std::string ret; /*TODO*/
	ret = codecid_name[codecid];
	return ret;
}

codecid_t codecid_from_name(const std::string& name) {
	for (const auto& map_pair : codecid_name) {
		if (name == map_pair.second)
			return map_pair.first;
	}
	return codecid_other;
}
