#include <cstdlib>
#include <vector>
#include <string>
#include <functional>
#include <glib.h>
#include <gio/gio.h>
#include "model.h"
#include "mediaconvert.h"

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

int mediaconvert::convert(size_t i, std::string& outstring) {
	std::string outstring;
	int         code;
	
	assert(i >= 0);
	assert(i < elementv.size());
	
	media_t& elem = elementv[i];
	
	std::vector<std::string> extract_argv;
	extract_argv.emplace_back("mkvextract");
	extract_argv.emplace_back("--command-line-charset");
	extract_argv.emplace_back("UTF-8");
	extract_argv.emplace_back("--output-charset");
	extract_argv.emplace_back("UTF-8");
	extract_argv.emplace_back(elem.path);
	
	code = launchprocess(extract_argv, outstring);
	if (code != 0)
		return -1;
	
	std::vector<std::string> convert_argv;
	std::string converter = settings::pt.get("converter", "");
	if (converter == "ac3to") {
		convert_argv.emplace_back("eac3to");
		convert_argv.emplace_back("....ac3");
		convert_argv.emplace_back("....aac");
	}
	else {
		convert_argv.emplace_back("ffmpeg");
		convert_argv.emplace_back("....ac3");
		convert_argv.emplace_back("....aac");
	}
	
	code = launchprocess(convert_argv, outstring);
	if (code != 0)
		return -1;
	
	std::vector<std::string> merge_argv;
	merge_argv.emplace_back("mkvmerge");
	merge_argv.emplace_back("");
	merge_argv.emplace_back("");
	
	code = launchprocess(merge_argv, outstring);
	if (code != 0)
		return -1;
	
	std::vector<std::string> edit_argv;
	edit_argv.emplace_back("mkvpropedit");
	edit_argv.emplace_back("");
	edit_argv.emplace_back("");
	
	code = launchprocess(edit_argv, outstring);
	if (code != 0)
		return -1;
	
	return 0;
}

