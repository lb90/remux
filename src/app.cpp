#include <cstdlib>
#include <cassert>
#include <ctime>
#include <vector>
#include <string>
#include "signalcentre.h"
#include "settings.h"
#include "glibutil.h"
#include "model.h"
#include "scandirectory.h"
#include "dialogproperty.h"
#include "dialogconversion.h"
#include "window.h"
#include "app.h"

unsigned app::checker_id;

std::string app::install_dir;

std::string app::mkvtoolnix_dir;
std::string app::ffmpeg_dir;

std::string app::mkvextract_prog;
std::string app::mkvmerge_prog;
std::string app::ffmpeg_prog;

bool app::has_timer;
int app::hour;
int app::minute;
	
bool app::on_other_day;
int app::year;
int app::month;
int app::day;

bool        app::showwindow;

GtkApplication *app::gtkapp;
dialogproperty_t *app::dialogproperty = nullptr;

std::vector<std::string> app::subtitletags;

void app::scandirectory(const char *directoryname) {
	scan_directory(directoryname);
}

void app::elementactivated(int n) {
	assert(n >= 0);
	assert(size_t(n) < elementv.size());
	dialogproperty_t *dialog = new dialogproperty_t(get_window(NULL));
	
	dialog->setcurrentelement(n);
	dialog->show();
}

int app::check_conversion_timer(void*) {
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);

    if ( (lt->tm_hour >  hour) ||
         (lt->tm_hour == hour && lt->tm_min >= minute) ) {
        
        if (on_other_day) {
            if (lt->tm_year == year &&
                lt->tm_mon == month &&
                lt->tm_mday == day) {

                return TRUE;
            }
        }

        has_timer = false;
        dialogconversion *dialog = new dialogconversion(get_window(NULL));
    	dialog->show();
    	return FALSE;
    }
    
    return TRUE;
}

void app::set_conversion_timer(int h, int m) {
    assert(h >= 0);
    assert(m >= 0);
    assert(h < 24);
    assert(m < 60);
    
    bool already_had_timer = has_timer;
    
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    
    has_timer = true;
    hour = h;
    minute = m;
    if (  (hour >  (lt->tm_hour) )
       || (hour == (lt->tm_hour) && minute > (lt->tm_min)) )
        on_other_day = false;
    else {
        on_other_day = true;

        year  = lt->tm_year;
        month = lt->tm_mon;
        day   = lt->tm_mday;
    }
    if (!already_had_timer)
        checker_id = g_timeout_add_seconds(1, check_conversion_timer, NULL);
}

void app::remove_conversion_timer() {
    g_source_remove(checker_id);
    checker_id = 0;
    
    has_timer = false;
}

int app::init() {
	signalcentre::addhandler("element\\activate", app::elementactivated);
	
#ifdef _WIN32
	install_dir = util_install_directory();
#endif

	mkvtoolnix_dir = settings::pt.get("dir.mkvtoolnix", "");
	ffmpeg_dir     = settings::pt.get("dir.ffmpeg", "");

#ifdef _WIN32	
	if (mkvtoolnix_dir.empty()) {
		std::string intree = util_build_filename(install_dir, "mkvtoolnix");
		if (g_file_test(intree.c_str(), G_FILE_TEST_IS_DIR))
			mkvtoolnix_dir = intree;
	}
	if (ffmpeg_dir.empty()) {
		std::string intree = util_build_filename(install_dir, "ffmpeg");
		if (g_file_test(intree.c_str(), G_FILE_TEST_IS_DIR))
			ffmpeg_dir = intree;
	}
#endif
	
	if (mkvtoolnix_dir.empty()) {
		mkvextract_prog = "mkvextract";
		mkvmerge_prog = "mkvmerge";
	}
	else {
		mkvextract_prog = util_build_filename(mkvtoolnix_dir, "mkvextract");
		mkvmerge_prog = util_build_filename(mkvtoolnix_dir, "mkvmerge");
	}
	
	if (ffmpeg_dir.empty())
		ffmpeg_prog = "ffmpeg";
	else
		ffmpeg_prog = util_build_filename(ffmpeg_dir, "ffmpeg");
	
#ifdef _WIN32
	mkvextract_prog += ".exe";
	mkvmerge_prog += ".exe";
	ffmpeg_prog += ".exe";
#endif
	try {
		const auto& childpt = settings::pt.get_child("tags.subtitle");
		for (const auto& item : childpt) {
			if (item.first == "tag")
				subtitletags.emplace_back(item.second.data());
		}
	}
	catch (boost::property_tree::ptree_error e) {
	}
	
	showwindow = settings::pt.get("showwindow", false);

	return 0;
}

