#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include "glibutil.h"
#include "elements.h"
#include "parseinfo.h"
#include "launchprocess.h"
#include "mediascan.h"

int media_scan(media_t& elem) {
	int          code = 0;
	
	elem.isinit = true;
	
	std::vector<std::string> argv;
	std::string outputstring;
	
	argv.emplace_back("mkvinfo");
	argv.emplace_back("--ui-language");
	argv.emplace_back("en_US");
	argv.emplace_back("--command-line-charset");
	argv.emplace_back("UTF-8");
	argv.emplace_back("--output-charset");
	argv.emplace_back("UTF-8");
	argv.emplace_back(elem.path);

	code = launch_process(argv, outputstring, true);
	if (code < 0) {
		elem.err.scan = true;
		elem.err.scan_description = outputstring;
	}
	else {
		std::stringstream sstream(outputstring);
		parse_info(sstream, elem.pt);
	}

	return 0;
}

