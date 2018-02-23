#ifndef REMUX_APP_H
#define REMUX_APP_H

#include <vector>
#include <string>
#include <gtk/gtk.h>
#include "dialogproperty.h"

class app {
public:
	static int init();

	static void scandirectory(const char *directoryname);
	static void elementactivated(int n);
	
	static void set_conversion_timer(int h, int m);

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
	
	static
	GtkApplication *gtkapp;
	
	static
	dialogproperty_t *dialogproperty;
	
	static bool has_timer;
	static int hour;
	static int minute;
	
	static bool on_other_day;
	static int year;
	static int month;
	static int day;
};

#endif
