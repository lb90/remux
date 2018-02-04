#include <cstdlib>
#include <string>
#include "signalcentre.h"
#include "settings.h"
#include "glibutil.h"
#include "model.h"
#include "scandirectory.h"
#include "dialogproperty.h"
#include "window.h"
#include "app.h"

#ifdef _WIN32
std::string app::install_dir;
std::string app::ac3to_dir;
std::string app::ac3to_prog;
#endif
std::string app::mkvtoolnix_dir;
std::string app::mkvinfo_prog;
std::string app::mkvextract_prog;
std::string app::mkvmerge_prog;
std::string app::mkvpropedit_prog;
std::string app::ffmpeg_dir;
std::string app::ffmpeg_prog;

void app::scandirectory(const char *directoryname) {
	model_clear();
	scan_directory(directoryname);
}

void app::elementactivated(int n) {
	assert(n >= 0);
	assert(size_t(n) < elementv.size());
	dialogproperty_t *dialog = new dialogproperty_t(get_window(NULL));
	
	dialog->setcurrentelement(n);
	dialog->show();
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
		mkvinfo_prog = "mkvinfo";
		mkvextract_prog = "mkvextract";
		mkvmerge_prog = "mkvmerge";
		mkvpropedit_prog = "mkvpropedit";
	}
	else {
		mkvinfo_prog = util_build_filename(mkvtoolnix_dir, "mkvinfo");
		mkvextract_prog = util_build_filename(mkvtoolnix_dir, "mkvextract");
		mkvmerge_prog = util_build_filename(mkvtoolnix_dir, "mkvmerge");
		mkvpropedit_prog = util_build_filename(mkvtoolnix_dir, "mkvpropedit");
	}
	
	if (ffmpeg_dir.empty())
		ffmpeg_prog = "ffmpeg";
	else
		ffmpeg_prog = util_build_filename(ffmpeg_dir, "ffmpeg");
	
#ifdef _WIN32
	mkvinfo_prog += ".exe";
	mkvextract_prog += ".exe";
	mkvmerge_prog += ".exe";
	mkvpropedit_prog += ".exe";
	ffmpeg_prog += ".exe";
#endif

	return 0;
}

