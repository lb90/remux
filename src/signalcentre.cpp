#include <cstdlib>
#include <cassert>
#include <map>
#include <string>
#include <functional>
#include "signalcentre.h"

std::multimap<std::string, func_t> signalcentre::signalmap;

int signalcentre::emit(const std::string& name, int arg) {
	if (signalmap.count(name) == 0)
		return 0;
	
	auto eq_range = signalmap.equal_range(name);
	for (auto it = eq_range.first; it != eq_range.second; ++it)
		(it->second) (arg);

	return 0;
}

int signalcentre::addhandler(const std::string& name, func_t handler) {
	signalmap.emplace(name, handler);

	return 0;
}
/*
int signalcentre::removehandler(const std::string& name, func_t handler) {
	if (!name.empty()) {
		auto eq_range = signalmap.equal_range(name);
		for (auto it = eq_range.first; it != eq_range.second;)
			if (handler.target == (*it).second.target)
				it = signalmap.erase(it);
			else
				++it;
	}
	else
		for (auto it = signalmap.begin(); it != signalmap.end();)
			if (handler.target == (*it).second.target)
				it = signalmap.erase(it);
			else
				++it;

	return 0;
}
*/

