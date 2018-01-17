#include <cstdlib>
#include <cassert>
#include <string>
#include <stack>
#include <boost/property_tree.hpp>
#include <sstream>
#include <boost/strings.hpp>

std::stack<boost::property_tree::ptree*> stackelem;

static
void pretty_line(std::string& line) {
	assert(line.size() <= INT_MAX);
	int imax = (int) line.size();
	for (int i = 0; i < imax; i++) {
		if (line[i] == ' ') continue;
		if (line[i] == '|') line[i] = ' ';
		else break;
	}
}

int makept(std::string& info) {
	std::sstream(info);
	std::string line;
	while (std::getline(info_sstream, line)) {
		if (line.empty()) continue;
		pretty_line(line);
		
		size_t ppos = line.find('+');
		if (ppos == std::string::npos)
			continue;
		
		assert(ppos < stackelem.size());
		
		*cur = stackelem.back();
		cur[
	}
}

int gatherfrom(pt) {
	if pt.child("EBML head") {
		if (!(child["Document type"] == "matroska"))
			return 1;
	}
	else return -1;
	/*TODO could be Segment: size xxxxxx... */
	if pt.child("Segment") {
		if pt.child("Segment information") {
			title = child.get("Title", "");
		}
		else return -1;
	}
	else return -1;
	
	title = pt.get("Segment.Segment information.Title", "");
	for (pt.getchild("Segment.Tracks.Track")) {
	}
}

int parseinfo(const std::string& info) {
	stackelem.push(&pt);
	makept(info);
	gatherfrom(pt);
}
