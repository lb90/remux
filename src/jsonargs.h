#ifndef REMUX_JSONARGS_H
#define REMUX_JSONARGS_H

#include <vector>
#include <string>
#include <ostream>

class jsonargs {
	std::vector<std::string> args;

	void push_isdefault(bool isdefault);
	void push_isforced(bool isforced);
	void push_trackname(const std::string& trackname);
	void push_language(const std::string& language);
	
	void push(const std::string& arg);
	
	void push_audio();
	void push_video();
	void push_subtitles();
	void push_button();
	
	void savejson(std::ostringstream sstream);
};

#endif
