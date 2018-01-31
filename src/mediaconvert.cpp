#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <stdexcept>
#include <functional>
#include <thread>
#include "app.h"
#include "model.h"
#include "launchprocess.h"
#include "mediaconvert.h"

std::string util_plain_comma_list(const std::vector<size_t>& indexv) {
	std::string commasequence;

	if (indexv.empty())
		throw std::logic_error("empty track-id list");
	
	commasequence = std::to_string(indexv[0]);
	for (size_t i = 1; i < indexv.size(); i++) {
		commasequence += "," + std::to_string(indexv[i]);
	}
	
	return 
}

int do_propedit(const media_t& elem, const item_t& item,
                convertcontext_t& convertctx)
{
	std::vector<std::string> argv;
	
	argv.emplace_back(app::mkvpropedit_prog);
	argv.emplace_back(convertctx.finalpath);

	if (elem.have_video) {
		argv.emplace_back("-e");
		argv.emplace_back("track:v1");
		argv.emplace_back("-s");
		argv.emplace_back("--set");
	}
	if (elem.have_audio) {
		argv.emplace_back("--edit");
		argv.emplace_back("--edit");
		argv.emplace_back("--edit");
	}
	if (elem.have_subtitles) {
		argv.emplace_back("--edit");
		argv.emplace_back("--edit");
		argv.emplace_back("--edit");
	}
}

int do_merge(const media_t& elem, const item_t& item,
                convertcontext_t& convertctx)
{
	std::vector<std::string> argv;
	int code;
	
	argv.push_back(app::mkvmerge_prog);
	argv.push_back("--command-line-charset");
#ifdef _WIN32
	argv.push_back("UTF-16");
#else
	argv.push_back("UTF-8");
#endif
	argv.push_back("--output-charset");
	argv.push_back("UTF-8");
	if (!elem.title.empty()) {
		argv.push_back("--title");
		argv.push_back(elem.title);
	}
	argv.push_back("-o")
	argv.push_back(convertctx.outname);
	
	
	
	
	if (convertctx.want_keep_audio_tid.empty())
		argv.push_back("-A");
	else {
		argv.push_back("-a");
		argv.push_back(util_plain_comma_list(convertctx.want_keep_audio_tid));
	}
	argv.push_back(elem.path);
	for (const std::string& outsider : convertctx.outsiders) {
		argv.emplace_back("--language:ita");
		argv.emplace_back("--default");
		argv.emplace_back("--forced");
		argv.emplace_back(outsider);
	}
	argv.push_back("--track-order");
	/*
	prima tutte le tracce video, poi gli audio convertiti (che sono tutti ita),
	poi gli audio ita che sono mantenuti, quindi tutto il resto
	*/
	std::string track_order;

	/*cerca tutte le tracce video*/
	std::vector<size_t> all_video_tids;
	for (const item_t& item : elem.items) {
		if (item.type == itemtype_video) {
			assert(item.tid >= 0);
			all_video_tids.push_back(item.tid);
		}
	}
	
	for (size_t videotid : all_video_tids)
		track_order += "0:" + std::to_string(videotid) + ",";
	
	for (size_t i = 0; i < convertctx.outsiders.size(); i++)
		track_order += std::to_string(i + 1) + ":0,";
	
	for (const item_t& item : elem.items) {
		if (item.type == itemtype_audio && item.langid == "ita") /*TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
			track_order += "0:" + std::to_string(item.tid) + ","
	}
	
	/*TODO remove final , in track_order string */
	code = launch_process(argv, outputstring);

	if (code != 0)
		return -1;
	
	/*TODO test existence of convertctx.outname file */
	
	return 0;
}

int do_keep(const item_t& item, convertcontext_t& convertctx) {
	convertcontext.keep_audio_tids.push_back(item.tid);
	
	return 0;
}

int do_convert(const media_t& elem, const item_t& item,
                convertcontext_t& convertctx)
{
	extractcontext_t extractctx;
	int code;
	
	code = do_extract(elem, item, extractctx);
	if (code != 0) {
		/*TODO do_cleanup */
		return -1;
	}
	
	code = do_reencode(extractctx);
	
	convertctx.outsiders.emplace_back(extractctx.outpath);
	
	return 0;
}

bool have_to_keep(const media_t& elem, const item_t& item) {
	if (elem.opt.want_keep_ac3 &&
	    item.type == itemtype_audio &&
	    item.codecid == codecid_ac3)
		return true;
	else if (elem.opt.want_keep_dolby &&
	         item.type == itemtype_audio &&
	         item.codecid == codecid_dolby)
		return true;
	else return false;
}

bool have_to_convert(const media_t& elem, const item_t& item) {
	if (elem.ok_convert_ac3ita_aac &&
	    elem.want_convert_ac3ita_aac)
		if (item.type == itemtype_audio &&
		    item.codecid == codecid_ac3 &&
		    item.langid == "it")
			return true;

	return false;
}

int mediaconvert::process(size_t n, std::string& outstring) {
	convertcontext_t convertctx;
	int code;

	assert(n < elementv.size());
	media_t& elem = elementv[n];
	
	for (size_t i = 0; i < elem.items.size(); i++) {
		const item_t& item = elem.items[i];

		if (item.type == itemtype_audio) {

			if (have_to_convert(elem, item) {				
				code = do_convert(elem, item, convertctx);
				if (code != 0)
					return -1;
			}
			else if (have_to_keep(elem, item) {			
				code = do_keep(elem, item, convertctx);
				if (code != 0)
					return -1;
			}
			
		}
	}
	
	code = do_merge(elem, convertctx);
	if (code != 0)
		return -1;
	code = do_propedit(elem, convertctx);
	if (code != 0)
		return -1;
	
	return 0;
}

void processall() {
	std::vector<size_t> indexv;

	for (size_t i = 0; i < elementv.size(); i++)
		if (elementv[i].ready)
			indexv.push_back(i);
	
	if (indexv.empty()) {
		//util_message("Non sono presenti media in coda");
		return;
	}
	
	for (size_t i : indexv) {
		std::string outstring;
		convert(i, outstring);
		//g_idle_add(util_callable_call, &progresscallcallback);
	}
}

void processall_start() {
	processall();
	
	g_idle_add(finishthreadcb);
}

bool mediaconvert::utilcallnotif(void *) {
	

	return FALSE;
}

bool mediaconvert::finishthreadcb(void*) {
	assert(t != nullptr);
	t->join();
	delete t;
	t = nullptr;
	
	return FALSE;
}

void mediaconvert::start() {
	if (t != nullptr)
		throw std::logic_error("convert process already started");
	t = new std::thread(processall_start);
}

