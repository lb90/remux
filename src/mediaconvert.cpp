#include <cstdlib>
#include <cassert>
#include <vector>
#include <deque>
#include <string>
#include <stdexcept>
#include <thread>
#include <glib.h>
#include <glib/gstdio.h>
#include "glibutil.h"
#include "jsonargs.h"
#include "launchprocess.h"
#include "logfile.h"
#include "mediaconvert.h"

mediaconvert::mediaconvert(std::deque<media_t*> c_elementd, int c_num_processes,
                           std::string c_ffmpeg_prog,
                           std::string c_mkvextract_prog,
                           std::string c_mkvmerge_prog)
 : progressd(),
   progressd_lock(),
   ctl(this),
   logfile(),
   ffmpeg_prog(c_ffmpeg_prog),
   mkvextract_prog(c_mkvextract_prog),
   mkvmerge_prog(c_mkvmerge_prog),
   m_elementd(c_elementd),
   m_num_processes(c_num_processes),
   threads(),
   m_state(exec_state_run),
   m_wanted_state(exec_state_run),
   unique_number_counter(0),
   m_tasks_count(0),
   thread_pool_lock()
{
}
/*
mediaconvert::convertcontext::convertcontext(media_t& elem, int unique_number)
 : elem(elem),
   unique_number(unique_number),
   extraction_tmpfile_name(),
   conversion_tmpfile_prefix()
{
    
}
*/
mediaconvert::~mediaconvert() {
    m_wanted_state = exec_state_stop; /*TODO*/
    
    for (std::thread& thread : threads) {
        thread.join();
    }
    
    logfile.save();
}

void mediaconvert::start() {
    assert(threads.empty());

    for (int i = 0; i < m_num_processes; i++) {
        threads.emplace_back(&mediaconvert::worker_start, this);
        m_tasks_count++;
    }
}

mediaconvert::controller::controller(mediaconvert *mc)
 : mc(mc)
{
}

void mediaconvert::controller::start_stop() {
    mc->m_wanted_state = exec_state_stop;
}

bool mediaconvert::controller::completed_stop() {
    return (mc->m_tasks_count == 0);
}

int mediaconvert::do_convert(media_t& elem, destitem_t& item,
                             const std::string& extractedpath,
                             const std::string& convertedpath)
{
	std::vector<std::string> argv;
	int code;
	
	//communicate(progressdata(0, 0, nullptr, "conversione della traccia estratta"));
	
	argv.emplace_back(ffmpeg_prog);
	/* overwrite existing files, run unattended */
	argv.emplace_back("-y");
	argv.emplace_back("-i");
	argv.emplace_back(extractedpath);
	argv.emplace_back(convertedpath);
	
	std::string outputstring, errorstring;
	int status = 0;
	code = launch_process(argv, outputstring, errorstring, &status);
	if (code != 0) {
		elem.err.conv = true;
		elem.err.conv_description = "errore nell'apertura del processo";
		if (!outputstring.empty())
			elem.err.conv_description += ": " + outputstring;
		//communicate(progressdata(0, 0, nullptr, elem.err.conv_description));
		return -1;
	}
	if (status != 0) {
		elem.err.conv = true;
		elem.err.conv_description = "il processo è uscito con il codice di errore ";
		elem.err.conv_description += std::to_string(status);
		if (!outputstring.empty())
			elem.err.conv_description += ": " + outputstring;
		//communicate(progressdata(0, 0, nullptr, elem.err.conv_description));
		return -1;
	}
	//communicate(progressdata(0, 0, nullptr, "ok!"));

	return 0;
}

int mediaconvert::do_extract(media_t& elem, destitem_t& item,
                             const std::string& extractedpath) {
	std::vector<std::string> argv;
	int code;
	
	//communicate(progressdata(0, 0, nullptr, "estrazione traccia " + item.num));

	argv.emplace_back(mkvextract_prog);
	argv.emplace_back("--command-line-charset");
#ifdef _WIN32
	argv.emplace_back("UTF-16");
#else
	argv.emplace_back("UTF-8");
#endif
	argv.emplace_back("--output-charset");
	argv.emplace_back("UTF-8");

	argv.emplace_back(elem.path);
	argv.emplace_back("tracks");
	argv.emplace_back(std::to_string(item.tid)+":"+extractedpath);
	
	std::string outputstring, errorstring;
	int status = 0;
	code = launch_process(argv, outputstring, errorstring, &status);
	if (code != 0) {
		elem.err.conv = true;
		elem.err.conv_description = "errore nell'apertura del processo";
		if (!outputstring.empty())
			elem.err.conv_description += ": " + outputstring;
		//communicate(progressdata(0, 0, nullptr, elem.err.conv_description));
		return -1;
	}
	if (status == 1) {
		std::string warning;
		warning = "warning da mkvextract";
		if (!outputstring.empty())
			warning += ": " + outputstring;
		//communicate(progressdata(0, 0, nullptr, warning));
	}
	else if (status != 0) {
		elem.err.conv = true;
		elem.err.conv_description = "il processo è uscito con il codice di errore ";
		elem.err.conv_description += std::to_string(status);
		if (!outputstring.empty())
			elem.err.conv_description += ": " + outputstring;
		//communicate(progressdata(0, 0, nullptr, elem.err.conv_description));
		return -1;
	}
	//communicate(progressdata(0, 0, nullptr, "ok!"));

	return 0;
}

int mediaconvert::do_extract_convert(convertcontext& cvtctx, destitem_t& item) {
    media_t& elem = *cvtctx.p_elem;
	int code;
	
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

	std::string extractedpath = util_build_filename(elem.outdirectory,
	                                                cvtctx.extraction_tmpfile_name);
	std::string convertedpath = util_build_filename(elem.outdirectory,
	                                                cvtctx.conversion_tmpfile_prefix +
	                                                std::to_string(item.tid) +
	                                                extension);
	
	code = do_extract(elem, item, extractedpath);
	if (code != 0)
		return -1;
	code = do_convert(elem, item, extractedpath, convertedpath);

	g_remove(extractedpath.c_str()); /* cleanup temporary extraction file */

	if (code != 0)
		return -1;
		
	item.outpath = convertedpath;
	
	return 0;
}

int mediaconvert::check_do_extract_convert(convertcontext& cvtctx, destitem_t& item) {
	if (item.codecid != item.orig.codecid)
		return do_extract_convert(cvtctx, item);
	
	return 0;
}

void mediaconvert::process(convertcontext& cvtctx) {
    media_t& elem = *cvtctx.p_elem;
	int code;
	bool bcode;
	
	for (destitem_t& item : elem.destitems) {
		if (!item.want) /*TODO remove */
			continue;
		
		code = check_do_extract_convert(cvtctx, item);
		if (code != 0)
			return;
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
		//communicate(progressdata(0, 0, nullptr, elem.err.conv_description));
		return;
	}
		
	std::vector<std::string> argv;

	argv.push_back(mkvmerge_prog);
#ifndef _WIN32
	argv.push_back("--command-line-charset");
	argv.push_back("UTF-8");
	argv.push_back("--output-charset");
	argv.push_back("UTF-8");
#endif
	argv.push_back("@"+jsonpath);
	
	//communicate(progressdata(0, 0, nullptr, "merge finale"));
	
	std::string outputstring, errorstring;
	int status = 0;
	code = launch_process(argv, outputstring, errorstring, &status);
	if (code != 0) {
		elem.err.conv = true;
		elem.err.conv_description = "errore nell'apertura del processo";
		if (!outputstring.empty())
			elem.err.conv_description += ": " + outputstring;
		//communicate(progressdata(0, 0, nullptr, elem.err.conv_description));
		return;
	}
	if (status == 1) {
		std::string warning;
		warning = "warning da mkvmerge";
		if (!outputstring.empty())
			warning += ":\n" + outputstring;
		//communicate(progressdata(0, 0, nullptr, warning));
	}
	else if (status != 0) {
		elem.err.conv = true;
		elem.err.conv_description = "il processo è uscito con il codice di errore ";
		elem.err.conv_description += std::to_string(status);
		if (!outputstring.empty())
			elem.err.conv_description += ": " + outputstring;
		//communicate(progressdata(0, 0, nullptr, elem.err.conv_description));
		return;
	}
	
	//communicate(progressdata(0, 0, nullptr, "ok!"));
}

void mediaconvert::communicate(const progressdata& commdata) {
	std::lock_guard<std::mutex> lock(progressd_lock);
	progressd.emplace_back(commdata);
}
/*
void mediaconvert::do_processall() {
	std::vector<size_t> indexv;

	for (size_t i = 0; i < elementv.size(); i++) {
		media_t& elem = elementv[i];
		if (elem.isready && !elem.err.scan)
			indexv.push_back(i);
	}
	
	for (size_t i = 0; i < indexv.size(); i++) {
		size_t idx = indexv[i];
		media_t& elem = elementv[idx];
		
		g_thread_pool_push(pool, &elem, NULL);
	}

	assert(indexv.size() < INT_MAX);
	communicate(progressdata(int(indexv.size()), int(indexv.size()), nullptr));
	
	
}
*/
int mediaconvert::interlocked_get_context(convertcontext *ctx) {
    std::lock_guard<std::mutex> lock(thread_pool_lock);
    
    if (m_elementd.empty())
        return 1;
    
    ctx->p_elem = m_elementd.front();
    m_elementd.pop_front();

    ctx->unique_number = unique_number_counter++;
    ctx->extraction_tmpfile_name = "extract_tmpfile_" + std::to_string(ctx->unique_number);
    ctx->conversion_tmpfile_prefix = "conversion_tmpfile_" + std::to_string(ctx->unique_number) + "_track_";
    
    return 0;
}

void mediaconvert::worker_start() {
    while (true) {
        convertcontext cvtctx;

	    if (m_wanted_state == exec_state_stop)
	        break;

	    if (mediaconvert::interlocked_get_context(&cvtctx) > 0)
	        break;
	    
	    media_t& elem = *cvtctx.p_elem;
	    
	    logfile.started(elem.name); /* <-- TODO */
	    mediaconvert::process(cvtctx);
	    logfile.ended(elem.err.conv_description, elem.err.conv ? 2 : 0);
	    
	    /* cleanup */
	    for (const destitem_t& item : elem.destitems) {
		    if (!item.outpath.empty())
		        g_remove(item.outpath.c_str());
	    }
	    
	    communicate(progressdata(&elem));
	}
	
	m_tasks_count--;
}

