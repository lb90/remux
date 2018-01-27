#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <functional>
#include <glib.h>
#include <gio/gio.h>
#include <boost/algorithm/string.hpp>
#include "model.h"
#include "mediaconvert.h"

mediaconvert::convert_context_t::convert_context_t(media_t& elem)
 : elem(elem),
   outfile_ac3(),
   outfile_aac(),
   ac3itaidx()
   producedv()
{
	outfile_ac3 = util_build_filename(settings::work_dir, "temp.ac3");
	outfile_aac = util_build_filename(settings::work_dir, "temp.aac");
	
	if (elem.options.ac3ita_aac) {
		ac3ita_tid = util_get_ac3ita_track(elem);
		if (ac3ita_tid.empty())
			elem.options.ac3ita_aac = false;
	}
}

gboolean util_callable_call(gpointer userdata) {
	callable_t *callable = (callable_t*) userdata;
	
	(*callable)();
	
	delete callable;
}

int mediaconvert::launchprocess(const std::vector<std::string>& argv,
                                std::vector& outstring) {
	GSubprocess *subproc = NULL;
	GError      *errspec = NULL;
	
	std::vector<const char*> c_array;
	
	for (const auto& arg : argv)
		c_array.push_back(arg.c_str());
	c_array.push_back(NULL);

	subproc = g_subprocess_newv(c_array.data(),
	                            G_SUBPROCESS_FLAGS_STDOUT_PIPE,
	                            &errspec);

	if (subproc == NULL) {
		outstring = (errspec && errspec->message && errspec->message[0]) ?
		               errspec->message : "Errore sconosciuto";
		return -1;
	}
	else {
		if (!g_subprocess_wait(subproc, NULL, NULL)) {
			g_print("error in subprocess\n");
			outstring = "Errore sconosciuto";
			return -1;
		}
		else {
			int process_exitstatus;
			std::stringstream process_stdout;

			get_stdout_sstream(subproc, process_stdout); /*TODO or stderr */
			process_exitstatus = g_subprocess_get_exit_status(subproc);
			
			if (process_exitstatus != 0) {
				outstring = "errror. exit code: " + std::to_string(process_exitstatus) + "\n";
				outstring += process_stdout.str();
				return -1;
			}
			else {
				outstring += process_stdout.str();
				return 0;
			}
		}
	}
}

void mediaconvert::convert(std::function) {
	std::vector<size_t> indexv;

	for (size_t i = 0; i < elementv.size(); i++)
		if (elementv[i].ready)
			indexv.push_back(i);
	
	if (indexv.empty()) {
		util_message("Non sono presenti media in coda");
		return;
	}
	
	for (size_t i : indexv) {
		std::string outstring;
		convert(i, outstring);
		g_idle_add(util_callable_call, );
	}
}

int mediaconvert::set_attributes(std::string& outstring) {
	int code;

	std::vector<std::string> edit_argv;
	edit_argv.emplace_back("mkvpropedit");
	edit_argv.emplace_back("-e");
	edit_argv.emplace_back("track:=" + uid);
	edit_argv.emplace_back("-s");
	edit_argv.emplace_back("flag-default=1");
	edit_argv.emplace_back("-s");
	edit_argv.emplace_back("flag-forced=1");
	
	code = launchprocess(edit_argv, outstring);
	if (code != 0)
		return -1;
	
	return 0;
}

void mediaconvert::select_wanted_audio_tracks(std::vector<int>& want_tid) {
	for (const auto& item : cvtctx.elem.itemv) {
		if (item.itemtype == ITEMTYPE_AUDIO) {
			switch (item.codec_id) {
				case CODEC_ID_AC3:
					if (item.opt.intact_ac3)
						want_tid.push_back(item.tid);
					break;
				case CODEC_ID_DOLBY:
					if (item.opt.intact_dts)
						want_tid.push_back(item.tid);
					break;
				default:
					break;
			}
		}
	}
}

int util_get_first_video_track(media_t& elem) {
	for (const item_t& item : elem.itemv)
		if (item.itemtype == ITEMTYPE_VIDEO)
			return item.tid;

	return -1;
}

int mediaconvert::remove_add_tracks(std::string& outstring) {
	int code;

	std::vector<std::string> merge_argv;
	merge_argv.emplace_back("mkvmerge");
	merge_argv.emplace_back("-o");
	merge_argv.emplace_back(cvtctx.elem.outpath);

	merge_argv.emplace_back("-a")
	
	std::vector<int> want_tid;
	select_wanted_audio_tracks(want_tid);
	assert(!want_tid.empty()); /*TODO what if we have no audio tracks? */
	
	std::string compact_want_audio_tid;
	for (size_t i = 0; i+1 < want_tid.size(); i++)
		compact_want_audio_tid += std::to_string(tid) + ",";
	compact_want_audio_tid += std::to_string(want_tid.back());

	merge_argv.emplace_back(compact_want_audio_tid);
	merge_argv.emplace_back(cvtctx.elem.path);
	
	for (const std::string& producedfile : cvtctx.producedv)
		merge_argv.emplace_back(cvtctx.elem.path);
	
	/*first video track, then ita audio track, then the rest */
	std::string trackorder;
	int videotid = util_get_first_video_track(elem)
	if (videotid >= 0)
		trackorder += "0:" + std::to_string(videotid);
	/*TODO add audio track (if it's there...) */
	merge_argv.emplace_back();
	if (!trackorder.empty()) {
		merge_argv.emplace_back("--track-order");
		merge_argv.emplace_back(trackorder);
	}
	
	code = launchprocess(merge_argv, outstring);
	if (code != 0)
		return -1;
	
	return 0;
}

std::string mediaconvert::util_get_ac3ita_track(convert_context_t& cvtctx) {
	for (const auto& item : cvtctx.elem.itemv) {
		if (item.codec == "AC3") {
			std::string lang = item.language.substr(0, 2);
			boost::algorithm::to_lower(lang);
			if (lang == "it")
				return item.number; /*TODO number or UID? */
		}
	}

	return "";
}

int mediaconvert::ac3ita_aac_convert(std::string& outstring) {
	int code;
	
	std::string converter = settings::pt.get("converter", "");

	std::vector<std::string> convert_argv;
	if (converter == "ac3to") {
		convert_argv.emplace_back("eac3to");
		convert_argv.emplace_back(cvtctx.outfile_ac3);
		convert_argv.emplace_back(cvtctx.outfile_aac);
	}
	else { /*TODO specify codec  */
		convert_argv.emplace_back("ffmpeg");
		convert_argv.emplace_back("-i");
		convert_argv.emplace_back(cvtctx.outfile_ac3);
		convert_argv.emplace_back(cvtctx.outfile_aac);
	}
	
	code = launchprocess(convert_argv, outstring);
	if (code != 0)
		return -1;
	
	return 0;
}

int mediaconvert::ac3ita_aac_extract(std::string& outstring) {
	int code;

	std::vector<std::string> extract_argv;
	extract_argv.emplace_back("mkvextract");
	extract_argv.emplace_back("--command-line-charset");
	extract_argv.emplace_back("UTF-8");
	extract_argv.emplace_back("--output-charset");
	extract_argv.emplace_back("UTF-8");
	extract_argv.emplace_back(elem.path);
	extract_argv.emplace_back("tracks");
	extract_argv.emplace_back(ac3ita_tid + ":" + outfile_ac3);
	
	code = launchprocess(extract_argv, outstring);
	if (code != 0)
		return -1;
	
	return 0;
}

int mediaconvert::ac3ita_aac(std::string& outstring) {
	int code;

	code = ac3ita_aac_extract(outstring);
	if (code < 0) /*TODO can be 1 */
		return -1;
	
	code = ac3ita_aac_convert(outstring);
	if (code != 0)
		return -1;
	
	return 0;
}

int mediaconvert::convert(size_t i, std::string& outstring) {
	std::string outstring;
	int         code;
	
	assert(i >= 0);
	assert(i < elementv.size());
	
	media_t& elem = elementv[i];
	
	if (elem.options.ac3ita_aac) {
		code = mediaconvert::ac3ita_aac(outstring);
		if (code != 0)
			return -1;
	}
	
	code = mediaconvert::remove_add_tracks(outstring);
	if (code != 0)
		return -1;
	
	code = mediaconvert::set_attributes(outstring);
	if (code != 0)
		return -1;
	
	return 0;
}

