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
	class convert_context_t {
	public:
		convert_context_t(media_t& elem);

		media_t&    elem;
		std::string outfile_ac3;
		std::string outfile_aac;
		size_t      ac3itaidx;
		
		std::vector<std::string> producedv;
	};

	std::thread                   thread;
	std::function<void(int, int)> progresscallback;

	static void convert(size_t i);
};

#endif

