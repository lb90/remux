#ifndef REMUX_APP_H
#define REMUX_APP_H

#include <vector>
#include <string>

class app {
public:
	static int init();

	static void scandirectory(const char *directoryname);
	static void elementactivated(int n);

	static
	std::string install_dir; /* only used in win32 */

	static
	std::string mkvtoolnix_dir;
	static
	std::string ffmpeg_dir;

	static
	std::string mkvextract_prog;
	static
	std::string mkvmerge_prog;
	static
	std::string ffmpeg_prog;
	
	static
	std::vector<std::string> subtitletags;
	
	static
	bool showwindow;
};

#endif
