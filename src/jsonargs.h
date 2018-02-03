#ifndef REMUX_JSONARGS_H
#define REMUX_JSONARGS_H

#include <vector>
#include <string>
#include <sstream>

class jsonargs {
	std::vector<std::string> args;

public:
	void push_output(const std::string& output);
	void push_title(const std::string& title);

	void push_isdefault(bool isdefault);
	void push_isforced(bool isforced);
	void push_trackname(const std::string& trackname);
	void push_language(const std::string& language);
	
	void push(const std::string& arg);
	
	void push_only_video();
	void push_only_audio();
	void push_only_subtitle();
	void push_only_button();
	
	void push_video_tid(int tid);
	void push_audio_tid(int tid);
	void push_subtitle_tid(int tid);
	void push_button_tid(int tid);
	
	void savejson(std::stringstream& sstream);
};

#endif
