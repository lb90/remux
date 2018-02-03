#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "jsonargs.h"

/* https://stackoverflow.com/questions/7724448/simple-json-string-escape-for-c */
static
std::string util_escape_json(const std::string &s) {
    std::ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        switch (*c) {
        case '"': o << "\\\""; break;
        case '\\': o << "\\\\"; break;
        case '\b': o << "\\b"; break;
        case '\f': o << "\\f"; break;
        case '\n': o << "\\n"; break;
        case '\r': o << "\\r"; break;
        case '\t': o << "\\t"; break;
        default:
            if ('\x00' <= *c && *c <= '\x1f') {
                o << "\\u"
                  << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
            } else {
                o << *c;
            }
        }
    }
    return o.str();
}

void jsonargs::push_isdefault(bool isdefault) {
	args.emplace_back("--default-track");
	if (isdefault)
		args.emplace_back("0:1");
	else
		args.emplace_back("-1:0");
}

void jsonargs::push_isforced(bool isforced) {
	args.emplace_back("--forced-track");
	if (isforced)
		args.emplace_back("0:1");
	else
		args.emplace_back("-1:0");
}

void jsonargs::push_trackname(const std::string& trackname) {
	args.emplace_back("--track-name"); /*TODO review*/
	args.emplace_back("-1:" + trackname);
}

void jsonargs::push_language(const std::string& language) {
	args.emplace_back("--language");
	args.emplace_back("-1:" + language);
}

void jsonargs::push(const std::string& arg) {
	args.emplace_back(arg);
}

void jsonargs::push_only_audio() {
	args.emplace_back("-D"); /*TODO*/
	args.emplace_back("-S");
	args.emplace_back("-B");
}

void jsonargs::push_only_video() {
	args.emplace_back("-A");
	args.emplace_back("-S");
	args.emplace_back("-B");
}

void jsonargs::push_only_subtitle() {
	args.emplace_back("-A");
	args.emplace_back("-D");
	args.emplace_back("-B");
}

void jsonargs::push_only_button() {
	args.emplace_back("-A");
	args.emplace_back("-D");
	args.emplace_back("-S");
}

void jsonargs::push_audio_tid(int tid) {
	args.emplace_back("-a");
	args.emplace_back(std::to_string(tid));
}

void jsonargs::push_video_tid(int tid) {
	args.emplace_back("-d");
	args.emplace_back(std::to_string(tid));
}

void jsonargs::push_subtitle_tid(int tid) {
	args.emplace_back("-s");
	args.emplace_back(std::to_string(tid));
}

void jsonargs::push_button_tid(int tid) {
	args.emplace_back("-b");
	args.emplace_back(std::to_string(tid));
}

void jsonargs::push_title(const std::string& title) {
	args.emplace_back("--title");
	args.emplace_back(title);
}

void jsonargs::push_output(const std::string& output) {
	args.emplace_back("-o");
	args.emplace_back(output);
}

void jsonargs::savejson(std::stringstream& sstream) {
	sstream << "[\n";
	for (size_t i = 0; i < args.size(); i++) {
		sstream << "  ";
		sstream << "\"";
		sstream << util_escape_json(args[i]);
		sstream << "\"";
		if (i +1 != args.size())
			sstream << ",";
		sstream << "\n";
	}
	sstream << "]\n";
}

