#ifndef REMUX_SIGNALCENTRE_H
#define REMUX_SIGNALCENTRE_H

#include <map>
#include <string>
#include <functional>

typedef std::function<void(int)> func_t;

class signalcentre {
public:
	static
	int emit(const std::string& name, int arg);

	static
	int addhandler(const std::string& name, func_t func);
	/*static
	int removehandler(const std::string& name, func_t func);*/
private:
	static
	std::multimap<std::string, func_t> signalmap;
};

#endif
