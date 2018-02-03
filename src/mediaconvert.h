#ifndef REMUX_MEDIACONVERT_H
#define REMUX_MEDIACONVERT_H

#include <string>
#include <deque>
#include <thread>
#include <mutex>
#include "elements.h"

class mediaconvert {
public:
	struct progressdata_t {
		int         total;
		int         n;
		media_t    *elem;
		std::string text;
	};

	explicit     mediaconvert();
	void         start();

	std::deque<progressdata_t> progressd;
	std::mutex                 progressd_lock;

private:
	void         do_process(media_t& elem);
	void         do_processall();
	void         communicate(const progressdata_t& commdata);

	std::thread  worker;
	
	static void  worker_start(void*);
	static int   callback_worker_is_ending(void*);
};

#endif

