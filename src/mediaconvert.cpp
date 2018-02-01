#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <stdexcept>
#include <functional>
#include <thread>
#include "app.h"
#include "model.h"
#include "jsonargs.h"
#include "launchprocess.h"
#include "mediaconvert.h"

int mediaconvert::check_do_convert(media_t& elem) {
	for (destitem_t item : elem.destitems) {
		if (item.codec != item.orig.codec) {
			code = do_convert(item);
			if (code < 0)
				return -1;
		}
	}
	
	return 0;
}

void mediaconvert::do_processall() {
	std::vector<size_t> indexv;
	int code;

	for (size_t i = 0; i < elementv.size(); i++)
		if (elementv[i].ready)
			indexv.push_back(i);
	
	if (indexv.empty()) {
		//util_message("Non sono presenti media in coda");
		return;
	}
	
	for (size_t i : indexv) {
		media_t& elem = elementv[i];
		
		code = check_do_convert(elem);
		if (code != 0)
			return -1;
		
		g_idle_add(util_callable_call, &progresscallcallback);
	}
}

bool mediaconvert::callback_worker_is_ending(void*) {
	assert(t != nullptr);
	t->join();
	delete t;
	t = nullptr;
	
	return FALSE;
}

void mediaconvert::worker_start() {
	do_processall();
	
	g_idle_add(callback_worker_is_ending);
}

void mediaconvert::start() {
	if (t != nullptr)
		throw std::logic_error("convert process already started");
	t = new std::thread(worker_start);
}

