#ifndef REMUX_MEDIACONVERT_H
#define REMUX_MEDIACONVERT_H

#include <vector>
#include <string>
#include <thread>
#include <functional>

class mediaconvert {
public:
	static void convert();

private:
	std::thread                   thread;
	std::function<void(int, int)> progresscallback;

	static void convert(size_t i);
	
	struct convertcontext_t {
		std::vector<std::string> outsiders;
		std::vector<size_t> keep_audio_tids;
		std::vector<size_t> keep_video_tids;
	};
};

#endif

