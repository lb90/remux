#include <cstdlib>
#include <cassert>
#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include <glib.h>
#include <gio/gio.h>
#include "launchprocess.h"

void get_stdout_sstream(GSubprocess *subproc, std::stringstream& sstream) {
	GInputStream *istream;
	GError       *errspec;

	istream = g_subprocess_get_stdout_pipe(subproc);
	
	const size_t sz = 100;
	char array[sz];
	memset(array, 0, sz);
	for (;;) {
		size_t bread = 0;

		errspec = NULL;					
		if (!g_input_stream_read_all(istream, array, sz-1, &bread, NULL, &errspec)) {
			g_print("error reading stdout of child process");
			if (errspec) g_print(": %s", errspec->message);
			g_print("\n");
		}
		sstream << array;

		memset(array, 0, sz);
		if (bread < sz-1) break;
	}
}

int launch_process(const std::vector<std::string>& argv, std::string& outstring, bool want_stdout) {
	GSubprocess *subproc = NULL;
	GError      *errspec = NULL;
	int          ret = 0;
	
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
		ret = -1;
	}
	else {
		if (!g_subprocess_wait(subproc, NULL, NULL)) {
			g_print("error in subprocess\n");
			outstring = "Errore sconosciuto";
			ret = -1;
		}
		else {
			int exitstatus;
			std::stringstream proc_stdout;

			exitstatus = g_subprocess_get_exit_status(subproc);
			
			if (exitstatus != 0 || want_stdout) {
				get_stdout_sstream(subproc, proc_stdout);
				outstring += proc_stdout.str();
			}
			
			if (exitstatus != 0)
				ret = -1;
		}
	}
	
	if (subproc)
		g_object_unref(subproc);
	
	return ret;
}

