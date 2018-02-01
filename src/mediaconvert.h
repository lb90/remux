#ifndef REMUX_MEDIACONVERT_H
#define REMUX_MEDIACONVERT_H

#include <string>
#include <thread>
#include <functional>
#include "elements.h"

class mediaconvert {
public:
	class callback_t {
		std::function<void(int, int)>    newelement();
		std::function<void(std::string)> text();
		std::function<void(void)>        done();
	};
	//explicit     mediaconvert(const callback_t& callback);
	explicit     mediaconvert();
	void         start();
private:
	void         do_process(media_t& elem);
public:
	void         do_processall();
private:

	callback_t   callback;
	std::thread *worker;
	
	static void  worker_start(void*);
	static int   callback_worker_is_ending(void*);
};

#endif

