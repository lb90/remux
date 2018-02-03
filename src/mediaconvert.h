#ifndef REMUX_MEDIACONVERT_H
#define REMUX_MEDIACONVERT_H

#include <string>
#include <deque>
#include <thread>
#include <mutex>
#include "elements.h"

class mediaconvert {
public:
	struct progressdata {
		progressdata(int total,
		             int n,
		             media_t* elem,
		             std::string& text)
		 : total(total),
		   n(n),
		   elem(elem),
		   text(text)
		 { }

		int         total;
		int         n;
		media_t    *elem;
		std::string text;
	};

	explicit     mediaconvert();
	void         start();

	std::deque<progressdata> progressd;
	std::mutex               progressd_lock;

private:
	void         do_process(media_t& elem);
	void         do_processall();
	void         communicate(const progressdata& commdata);

	std::thread  worker;
	
	static void  worker_start(void*);
	static int   callback_worker_is_ending(void*);
};

#endif

