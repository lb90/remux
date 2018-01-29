#ifndef REMUX_APP_H
#define REMUX_APP_H

#include <string>

class app {
public:
	static int init();

	static void scandirectory(const char *directoryname);
	static void elementactivated(int n);

#ifdef _WIN32
	static
	std::string install_dir;
	static
	std::string ac3to_dir;
	static
	std::string ac3to_prog;
#endif
	static
	std::string mkvtoolnix_dir;
	static
	std::string ffmpeg_dir;
	static
	std::string mkvinfo_prog;
	static
	std::string mkvextract_prog;
	static
	std::string mkvmerge_prog;
	static
	std::string mkvpropedit_prog;
	static
	std::string ffmpeg_prog;
};

#endif
