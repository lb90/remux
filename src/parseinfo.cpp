#include <cstdlib>
#include <cassert>
#include <string>
#include <sstream>
#include <stack>
#include <algorithm>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "parseinfo.h"

typedef boost::property_tree::ptree pt_t;

bool check_indent(const std::string& line, size_t level) {
	assert(level < line.size());
	
	for (size_t count = 0; count < level; count++)
		if (line[count] != ' ' &&
		    line[count] != '|'   )
			return false;
	
	return true;
}

void parse_line(const std::string& line,
                std::pair<std::string, std::string>& kv)
{
	size_t pos = 0;
	
	pos = line.find(':');
	if (pos == std::string::npos) {
		kv.first = line;
		kv.second = "";
	}
	else {
		kv.first = line.substr(0, pos);
		kv.second = line.substr(pos + 1, std::string::npos);
	}
	
	boost::algorithm::trim(kv.first);
	boost::algorithm::trim(kv.second);
	
	std::replace(kv.first.begin(), kv.first.end(), ' ', '-');
}

int parse_info(std::stringstream& info,
              boost::property_tree::ptree& pt)
{
	std::stack<pt_t*> curitems;
	size_t prevlevel;
	size_t level;
	
	curitems.push(&pt);
	prevlevel = 0;
	
	std::string line;
	while (std::getline(info, line)) {
		if (line.empty()) continue;
		
		level = line.find('+');
		if (level == std::string::npos)
			continue;
		if (level > prevlevel + 1)
			return -1;
		
		if (!check_indent(line, level))
			return -1;
		
		if (level == prevlevel + 1)
			curitems.push(&(curitems.top()->back().second));

		else if (level < prevlevel)
			for (size_t count = 0; count < prevlevel - level; count++)
				curitems.pop();
		
		std::pair<std::string, std::string> kv;
		parse_line(line.substr(level + 1, std::string::npos), kv);
		curitems.top()->add(kv.first, kv.second);
		
		prevlevel = level;
	}
	
	return 0;
}

