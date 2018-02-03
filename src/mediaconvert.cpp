#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <stdexcept>
#include <thread>
#include <glib.h>
#include <glib/gstdio.h>
#include "glibutil.h"
#include "app.h"
#include "model.h"
#include "jsonargs.h"
#include "launchprocess.h"
#include "mediaconvert.h"

mediaconvert::mediaconvert()
 : progressd(),
   progressd_lock(),
   worker(worker_start, this)
 {}

int mediaconvert::do_convert_ffmpeg(media_t& elem, destitem_t& item,
                                   const std::string& extractedpath,
                                   const std::string& convertedpath)
{
	std::vector<std::string> argv;
	
	communicate(progressdata(0, 0, nullptr, "conversione con ffmpeg..."));
	
	argv.emplace_back(app::ffmpeg_prog);
	argv.emplace_back("-i");
	argv.emplace_back(extractedpath);
	argv.emplace_back(convertedpath);
	
	std::string outputstring;
	code = launch_process(argv, outputstring);
	if (code != 0) {
		elem.err.conv = true;
		elem.err.conv_description = outputstring;
		communicate(progressdata(0, 0, nullptr, outputstring));
		return -1;
	}
	communicate(progressdata(0, 0, nullptr, "ok!"));

	return 0;
}

#ifdef _WIN32
int mediaconvert::do_convert_ac3to(media_t& elem, destitem_t& item,
                                   const std::string& extractedpath,
                                   const std::string& convertedpath)
{
	std::vector<std::string> argv;
	
	communicate(progressdata(0, 0, nullptr, "conversione con eac3to..."));
	
	argv.emplace_back(app::ffmpeg_prog);
	argv.emplace_back(tmpextractpath);
	argv.emplace_back(item.outpath);
	
	std::string outputstring;
	code = launch_process(argv, outputstring);
	if (code != 0) {
		elem.err.conv = true;
		elem.err.conv_description = outputstring;
		communicate(progressdata(0, 0, nullptr, outputstring));
		return -1;
	}
	communicate(progressdata(0, 0, nullptr, "ok!"));

	return 0;
}
#endif

int mediaconvert::do_convert(media_t& elem, destitem_t& item) {
	std::string tid_string = std::to_string(item.tid);
	std::string base = "item_" + tid_string;

	std::string extension;
	switch (item.codecid) {
		case codecid_ac3:
			extension = ".ac3";
			break;
		case codecid_eac3:
			extension = ".eac3";
			break;
		case codecid_dts:
			extension = ".dts";
			break;
		case codecid_truehd:
			extension = ".thd";
			break;
		case codecid_aac:
			extension = ".aac";
			break;
		case codecid_mp2:
			extension = ".mp2";
			break;
		case codecid_mp3:
			extension = ".mp3";
			break;
		case codecid_vorbis:
			extension = ".ogg";
			break;
		case codecid_flac:
			extension = ".flac";
			break;
		case codecid_pcm:
			extension = ".wav";
			break;
		default:
			return -1;
	}

	item.outpath = util_build_filename(elem.outdirectory, base + extension);

#ifdef _WIN32
	if (item.orig.codecid == codecid_ac3 || item.orig.codecid == codecid_eac3) { /*TODO check ac3to can convert to type */
		return do_convert_ac3to(elem, item);
	}
#endif
	return do_convert_ffmpeg(elem, item);
}

int mediaconvert::do_extract(media_t& elem, destitem_t& item) {
	std::vector<std::string> argv;
	
	communicate(progressdata(0, 0, nullptr, "estrazione traccia " + item.num));

	argv.emplace_back(app::mkvextract_prog);
#ifdef _WIN32
	argv.emplace_back("--command-line-charset");
	argv.emplace_back("UTF-16");
#else
	argv.emplace_back("--command-line-charset");
	argv.emplace_back("UTF-8");
#endif
	argv.emplace_back("--output-charset");
	argv.emplace_back("UTF-8");

	argv.emplace_back(elem.path);
	argv.emplace_back("tracks");
	
	std::string tmpextractpath = util_build_filename(elem.outdirectory, "tmpextract"); /*TODO*/
	argv.emplace_back(tid_string + ":" + tmpextractpath);
	
	std::string outputstring;
	code = launch_process(argv, outputstring);
	if (code != 0) {
		elem.err.conv = true;
		elem.err.conv_description = outputstring;
		return -1;
	}

	return 0;
}

int mediaconvert::check_do_extract_convert(media_t& elem, destitem_t& item) {
	int code;

	if (item.codecid != item.orig.codecid) {
		code = do_extract(elem, item);
		if (code != 0)
			return -1;
		code = do_convert(elem, item);
		if (code != 0)
			return -1;
	}
}

void mediaconvert::do_process(media_t& elem) {
	int code;
	bool bcode;
	
	for (const destitem_t& item : elem.destitems) {
		if (!item.want)
			continue;
		
		check_do_extract_convert(elem, item);
	}
	
	jsonargs jargs;
	jargs.push_output(elem.outpath);
	
	jargs.push_title(elem.title);

	for (const destitem_t& item : elem.destitems) {
		if (!item.want)
			continue;

		if (item.outpath.empty()) {
			if (item.isdefault != item.orig.isdefault)
				jargs.push_isdefault(item.isdefault);
			if (item.isforced != item.orig.isforced)
				jargs.push_isforced(item.isforced);
			if (item.name != item.orig.name)
				jargs.push_trackname(item.name);
			if (item.lang != item.orig.lang)
				jargs.push_language(item.lang);
			
			switch (item.type) {
				case itemtype_video:
					jargs.push_only_video();
					jargs.push_video_tid(item.tid);
					break;
				case itemtype_audio:
					jargs.push_only_audio();
					jargs.push_audio_tid(item.tid);
					break;
				case itemtype_subtitle:
					jargs.push_only_subtitle();
					jargs.push_subtitle_tid(item.tid);
					break;
				case itemtype_button:
					jargs.push_only_button();
					jargs.push_button_tid(item.tid);
					break;
				default:
					throw std::logic_error("unknown item type");
			}
			
			jargs.push(elem.path);
		}
		else {
			jargs.push_isdefault(item.isdefault);
			jargs.push_isforced(item.isforced);
			if (!item.name.empty()) /*TODO can we give empty values? */
				jargs.push_trackname(item.name);
			if (!item.lang.empty())
				jargs.push_language(item.lang);

			switch (item.type) {
				case itemtype_video:
					jargs.push_only_video();
					break;
				case itemtype_audio:
					jargs.push_only_audio();
					break;
				case itemtype_subtitle:
					jargs.push_only_subtitle();
					break;
				case itemtype_button:
					jargs.push_only_button();
					break;
				default:
					throw std::logic_error("unknown item type");
			}
			
			jargs.push(item.outpath);
		}
	}

	std::stringstream sstream;
	GError *errspec = NULL;
	
	jargs.savejson(sstream);
	std::string jsonpath = util_build_filename(g_get_tmp_dir(), "remuxmkvargs.json"); /*TODO free?*/
	bcode = g_file_set_contents(jsonpath.c_str(),
	                            sstream.str().c_str(),
	                            -1,
	                            &errspec);
	if (bcode == FALSE) {
		elem.err.conv = true;
		elem.err.conv_description = "cannot create temporary json file";
		if (errspec && errspec->message && errspec->message[0]) {
			elem.err.conv_description += ": ";
			elem.err.conv_description += errspec->message;
		}
		elem.err.conv_description += "\n";
		return;
	}
		
	std::vector<std::string> argv;

	argv.push_back(app::mkvmerge_prog);
#ifndef _WIN32
	argv.push_back("--command-line-charset");
	argv.push_back("UTF-8");
	argv.push_back("--output-charset");
	argv.push_back("UTF-8");
#endif
	argv.push_back("@" + jsonpath);
	
	std::string outputstr;
	code = launch_process(argv, outputstr, false);
	if (code != 0) {
		elem.err.conv = true;
		elem.err.conv_description = "error in mkvmerge";
		if (!outputstr.empty()) {
			elem.err.conv_description += ": ";
			elem.err.conv_description += outputstr;
		}
		elem.err.conv_description += "\n";
		return;
	}
}

void mediaconvert::communicate(const progressdata_t& commdata) {
	std::lock_guard<std::mutex> lock(progressd_lock);
	progressd.emplace_back(commdata);
}

void mediaconvert::do_processall() {
	std::vector<size_t> indexv;
	int code;

	for (size_t i = 0; i < elementv.size(); i++) {
		media_t& elem = elementv[i];
		if (elem.isready && !elem.err.scan)
			indexv.push_back(i);
	}
	
	for (size_t i = 0; i < indexv.size(); i++) {
		size_t idx = indexv[i];
		media_t& elem = elementv[idx];
		
		assert(indexv.size() < INT_MAX);
		communicate(progressdata_t(int(indexv.size(), int(i), &elem, ""));
		
		do_process(elem);
		
		for (const destitem_t& item : elem.destitems) {
			if (!item.outpath.empty())
				g_remove(item.outpath.c_str());
		}
	}

	assert(indexv.size() < INT_MAX);
	communicate(progressdata(int(indexv.size()), int(indexv.size()), nullptr, ""));
}

int mediaconvert::callback_worker_is_ending(void* self) {
	mediaconvert *inst = (mediaconvert*) self;
	inst->worker.join();
	return FALSE;
}

void mediaconvert::worker_start(void *self) {
	mediaconvert *inst = (mediaconvert*) self;
	inst->do_processall();
	
	g_idle_add(callback_worker_is_ending, self);
}

