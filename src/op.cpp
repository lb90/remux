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
	std::string error_condition = elem.pt.get("errors", "");
	if (!error_condition.empty()) {
		elem.err.scan = true;
		elem.err.scan_description = error_condition;
		return;
	}
	
	if (!elem.pt.get("container.recognized", false)) {
		elem.err.scan = true;
		elem.err.scan_description = "format not recognized";
		return;
	}
	if (!elem.pt.get("container.supported", false)) {
		elem.err.scan = true;
		elem.err.scan_description = "format not suported";
		return;
	}

	std::string document_type = elem.pt.get("container.type", "");
	if (document_type != "Matroska") {
		elem.err.scan = true;
		elem.err.scan_description = "Not a matroska file";
		return;
	}

	elem.title = elem.pt.get("container.title", "");
	
	for (const auto& childpt : elem.pt.get_child("tracks")) { /*TODO*/
		if (childpt.first == "") {
			const boost::property_tree::ptree& trackpt = childpt.second;

			elem.origitems.emplace_back();
			origitem_t& item = elem.origitems.back();
			
			item.name = trackpt.get("properties.track_name", "");
			item.uid  = trackpt.get("properties.uid", "");
			item.num  = trackpt.get("properties.number", "");
			item.lang = trackpt.get("properties.language", "");

			std::string track_type;
			track_type = trackpt.get("type", "");
			if (track_type == "video") /*TODO*/
				item.type = itemtype_video;
			else if (track_type == "audio")
				item.type = itemtype_audio;
			else if (track_type == "subtitles")
				item.type = itemtype_subtitle;
			else if (track_type == "button") {
				item.type = itemtype_button;
			}
			else {
				elem.err.scan = true;
				elem.err.scan_description = "Invalid track type ("
				                            + track_type + ") "
				                            "for track with:\n"
				                            " - name: "
				                            + item.name + "\n"
				                            " - number: "
				                            + item.num + "\n"
				                            " - uid: "
				                            + item.uid + "\n";
				return;
			}
			
			item.tid = trackpt.get("id", -1);
			if (item.tid < 0) {
				elem.err.scan = true;
				elem.err.scan_description = "Invalid track id ("
				                            + std::to_string(item.tid)
				                             + ") "
				                            "for track with:\n"
				                            " - name: "
				                            + item.name + "\n"
				                            " - number: "
				                            + item.num + "\n"
				                            " - uid: "
				                            + item.uid + "\n";
				return;
			}
			
			item.codecname = trackpt.get("codec", "");
			if (item.codecname == "AC-3")
				item.codecid = codecid_ac3;
			else if (item.codecname == "EAC-3")
				item.codecid = codecid_ac3;
			else if (item.codecname == "DTS") /*TODO*/
				item.codecid = codecid_dolby;
			else if (item.codecname == "AAC")
				item.codecid = codecid_aac;
			else
				item.codecid = codecid_unknown;
			
			item.isdefault = trackpt.get("properties.default_track", true); /*TODO*/
			item.isforced  = trackpt.get("properties.forced_track", true); /*TODO*/
		}
	}
	
	for (const origitem_t& item : elem.origitems) {
		elem.destitems.emplace_back(item);
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

void op::enqueue(int n) {
	if (n < 0)
		return;

	assert(size_t(n) < elementv.size());
	model_enqueue(size_t(n));
}

void op::remove(std::vector<size_t>& indexv) {
	std::sort(indexv.begin(), indexv.end(), std::greater<size_t>());
	
	for (size_t i : indexv) {
		model_remove(i);
	}
}

