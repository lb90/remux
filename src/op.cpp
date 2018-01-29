#include <cstdlib>
#include <cassert>
#include <climits>
#include <algorithm>
#include "elements.h"
#include "model.h"
#include "mediascan.h"
#include "op.h"

/*TODO
consider using a case insensitive property tree
*/

static
void internal_fill_element(media_t& elem) {
	if (elem.pt.get("EBML_head.Document_type", "") != "matroska") {
		elem.err.scan = true;
		elem.err.scan_description = "Not a matroska file";
		return;
	}
	elem.title = elem.pt.get("Segment.Segment_information.Title", "");
	
	/*TODO (elem.pt.find("Segment.Tracks")*/
	for (const auto& childpt : elem.pt.get_child("Segment.Tracks")) {
		if (childpt.first == "Track") {
			elem.items.emplace_back();
			item_t& item = elem.items.back();
			
			/*TODO case insensitive comparison */
			std::string
			tracktype = childpt.second.get("Track_type", "");
			if (tracktype == "subtitles")
				item.type = itemtype_subtitle;
			else if (tracktype == "video")
				item.type = itemtype_video;
			else if (tracktype == "audio")
				item.type = itemtype_audio;
			else
				item.type = itemtype_unknown;
			
			item.name = childpt.second.get("Name", "");
			item.lang = childpt.second.get("Language", "");
			item.uid  = childpt.second.get("Track_UID", "");
			
			std::string
			number = childpt.second.get("Track_number", "");
			item.num = number.substr(0, number.find(' '));
			
			item.codecname = childpt.second.get("Codec_ID", "");
			/*TODO consider making case insensitive */
			if (item.codecname.find("AC3") != std::string::npos)
				item.codecid = codecid_ac3;
			else if (item.codecname.find("DTS") != std::string::npos)
				item.codecid = codecid_dolby;
			else if (item.codecname.find("AAC") != std::string::npos)
				item.codecid = codecid_aac;
			
			int isdefault = childpt.second.get("Default_track_flag", 0);
			item.orig_default = isdefault;
			item.want_default = isdefault;
			
			int isforced = childpt.second.get("Forced_track_flag", 0);
			item.orig_forced = isforced;
			item.want_forced = isforced;
		}
	}
}

void op::media_scan(int n) {
	assert(n >= 0);
	assert(size_t(n) < elementv.size());
	
	::media_scan(elementv[n]);
	if (elementv[n].err.scan)
		return;

	internal_fill_element(elementv[n]);
}

void op::remove(std::vector<size_t>& indexv) {
	std::sort(indexv.begin(), indexv.end(), std::greater<size_t>());
	
	for (size_t i : indexv) {
		model_remove(i);
	}
}

