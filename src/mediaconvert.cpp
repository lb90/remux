#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <functional>
#include <thread>
#include <glib.h>
#include "app.h"
#include "model.h"
#include "jsonargs.h"
#include "launchprocess.h"
#include "mediaconvert.h"

void mediaconvert::do_process(media_t& elem) {
	int code;
	bool bcode;
	
	jsonargs jargs;
	jargs.push_output(elem.outpath);

	for (const destitem_t& item : elem.destitems) {
		if (!item.want)
			continue;

		if (item.outpath.empty()) {
			if (item.isdefault != item.orig.isdefault)
				jargs.push_isdefault(item.isdefault);
			if (item.isforced != item.orig.isforced)
				jargs.push_isdefault(item.isdefault);
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
	bcode = g_file_set_contents("/tmp/remuxmkvargs.json", /*TODO*/
	                            sstream.str().c_str(), /*TODO*/
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
	argv.push_back("@/tmp/remuxmkvargs.json"); /*TODO*/
	
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

void mediaconvert::do_processall() {
	std::vector<size_t> indexv;
	int code;

	for (size_t i = 0; i < elementv.size(); i++)
		if (elementv[i].isready)
			indexv.push_back(i);
	
	/*if (indexv.empty())
		return;*/
	
	for (size_t i : indexv) {
		media_t& elem = elementv[i];
		
		do_process(elementv[i]);
		
		/* cleanup temporary files */
		/*for (const destitem_t& item : elem.destitems) {
			if (!item.outpath.empty())
				g_remove(item.outpath.c_str());
		}*/
		
		//g_idle_add(util_callable_call, &progresscallcallback);
	}
}

int mediaconvert::callback_worker_is_ending(void* self) {
	mediaconvert *inst = (mediaconvert*) self;

	assert(inst->worker != nullptr);
	inst->worker->join();
	delete inst->worker;
	inst->worker = nullptr;
	
	return FALSE;
}

void mediaconvert::worker_start(void *self) {
	mediaconvert *inst = (mediaconvert*) self;
	inst->do_processall();
	
	g_idle_add(callback_worker_is_ending, self);
}

void mediaconvert::start() {
	if (worker != nullptr)
		throw std::logic_error("convert process already started");
	worker = new std::thread(worker_start, this);
}

