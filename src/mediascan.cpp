#include <cstdlib>
#include <string>
#include <sstream>
#include <gio/gio.h>
#include "glibutil.h"
#include "elements.h"
#include "parseinfo.h"
#include "mediascan.h"

/*TODO
change g_print to rmx_log
*/

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

int media_scan(media_t& elem) {
	GSubprocess *subproc;
	GError      *errspec = NULL;
	
	elem.isinit = true;

	subproc = g_subprocess_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE,
	                           &errspec,
	                           "mkvinfo",
	                           "--ui-language",
                               "en_US",
	                           "--command-line-charset",
	                           "UTF-8",
	                           "--output-charset",
	                           "UTF-8",
	                           elem.path.c_str(),
	                           NULL);

	if (subproc == NULL) {
		g_print("error opening child process");
		if (errspec != NULL) g_print(": %s", errspec->message);
		g_print("\n");
		
		elem.errors.infoerror = true;
		elem.errors.infoerror_description = (errspec && errspec->message && errspec->message[0]) ?
		                                    errspec->message : "Errore sconosciuto";
	}
	else {
		if (!g_subprocess_wait(subproc, NULL, NULL)) {
			g_print("error in subprocess\n");
			elem.errors.infoerror = true;
			elem.errors.infoerror_description = "Errore sconosciuto";
		}
		else {
			int proc_exitstatus;
			std::stringstream proc_stdout;

			get_stdout_sstream(subproc, proc_stdout); /*TODO or stderr */
			proc_exitstatus = g_subprocess_get_exit_status(subproc);
			
			if (proc_exitstatus != 0) {
				/*g_print("child process exited with failure status %d\n", proc_exitstatus);*/
				elem.errors.infoerror = true;
				elem.errors.infoerror_description = proc_stdout.str();
			}
			else {
				parse_info(proc_stdout, elem.pt);
			}
		}
	}

	return 0;
}

