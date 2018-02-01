#ifndef REMUX_MEDIACONVERT_H
#define REMUX_MEDIACONVERT_H

#include <string>
#include <thread>
#include <functional>

class mediaconvert {
public:
	explicit     mediaconvert(const callback_t& callback);
	void         start();

private:
	void         do_processall();

	class callback_t {
		std::function<void(int, int)>    newelement();
		std::function<void(std::string)> text();
		std::function<void(void)>        done();
	};
	
	callback_t   callback;
	std::thread  worker;
	
	void         worker_start;
	bool         callback_worker_is_ending(void*);
};

#endif

