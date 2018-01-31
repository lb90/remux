#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "app.h"
#include "glibutil.h"
#include "elements.h"
#include "launchprocess.h"
#include "mediascan.h"

int media_scan(media_t& elem) {
	int          code = 0;
	
	elem.isinit = true;
	
	std::vector<std::string> argv;
	std::string outputstring;

	argv.emplace_back(app::mkvmerge_prog);
	argv.emplace_back("-J");
	argv.emplace_back(elem.path);

	code = launch_process(argv, outputstring, true);
	if (code < 0) {
		elem.err.scan = true;
		elem.err.scan_description = outputstring;
	}
	else {
		try {
			std::stringstream sstream(outputstring);
			boost::property_tree::read_json(sstream, elem.pt);
		}
		catch (boost::property_tree::json_parser::json_parser_error e) {
			elem.err.scan = true;
			elem.err.scan_description = "error parsing json\n";
			elem.err.scan_description += e.what();
		}
	}

	return 0;
}

