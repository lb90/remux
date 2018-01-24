#include <cstdlib>
#include <cassert>
#include <climits>
#include "elements.h"
#include "model.h"
#include "mediascan.h"
#include "op.h"

/*TODO
consider using a case insensitive property tree
*/

static
void internal_fill_element(media_t& elem) {
/*	if (elem.pt.get("EBML-head.Document-type", "") != "matroska") {
		elem.errors.infoerror = true;
		elem.errors.infoerror_description = "Not a matroska file";
		return;
	}*/
	elem.title = elem.pt.get("Segment.Title", "");
	
	if (!elem.pt.count("Segment.Tracks")) {
		/*TODO no tracks */
		return;
	}
	for (const auto& childpt : elem.pt.get_child("Segment.Tracks")) {
		if (childpt.first == "Track") {
			elem.itemv.emplace_back();
			item_t& item = elem.itemv.back();
			
			std::string tracktype_info = childpt.second.get("Track-type", "");
			if (tracktype_info == "subtitles")
				item.itemtype = ITEMTYPE_SUBTITLE;
			else if (tracktype_info == "video")
				item.itemtype = ITEMTYPE_VIDEO;
			else if (tracktype_info == "audio")
				item.itemtype = ITEMTYPE_AUDIO;
			else
				item.itemtype = ITEMTYPE_UNKNOWN;
			
			item.name = childpt.second.get("Name", "");
			item.language = childpt.second.get("Language", "");
			item.uid = childpt.second.get("Track-UID", "");
			
			std::string number_info = childpt.second.get("Track-number", "");
			item.number = number_info.substr(0, number_info.find(' '));
			
			item.codec = childpt.second.get("Codec-ID", "");
			
			int isdefault_info = childpt.second.get("Default-track-flag", -1);
			if (isdefault_info == -1) {
				elem.errors.infoerror = true;
				elem.errors.infoerror_description = "Cannot find info about default flag for track\n"
				                                    " Number = " + item.number + "\n"
				                                    " UID = " + item.uid + "\n"
				                                    " Name = " + item.name + "\n";
				return;
			}
			item.orig_default = isdefault_info;
			item.want_default = item.orig_default;
			
			int isforced_info = childpt.second.get("Forced-track-flag", -1);
			if (isforced_info == -1) {
				elem.errors.infoerror = true;
				elem.errors.infoerror_description = "Cannot find info about forced flag for track\n"
				                                    " Number = " + item.number + "\n"
				                                    " UID = " + item.uid + "\n"
				                                    " Name = " + item.name + "\n";
				return;
			}
			item.orig_forced = isforced_info;
			item.want_forced = item.orig_forced;
		}
	}
}

void op::media_scan(int n) {
	assert(n >= 0);
	assert(size_t(n) < elementv.size());
	
	::media_scan(elementv[n]);
	if (elementv[n].errors.infoerror)
		return;

	internal_fill_element(elementv[n]);
}
