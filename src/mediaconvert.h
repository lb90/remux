#ifndef REMUX_MEDIACONVERT_H
#define REMUX_MEDIACONVERT_H

#include <string>
#include <deque>
#include <thread>
#include <mutex>
#include "elements.h"
#include "logfile.h"

class mediaconvert {
public:
	struct progressdata {
		progressdata(int n_total,
		             int n_current,
		             media_t* elem)
		 : n_total(n_total),
		   n_current(n_current),
		   elem(elem)
		 { }

		int         n_total;
		int         n_current;
		media_t    *elem;
	};

	explicit mediaconvert();
	~mediaconvert();

	void         start();

	std::deque<progressdata> progressd;
	std::mutex               progressd_lock;
	GThreadPool             *pool;
	
	clslogfile logfile;

private:
	int          do_convert_ffmpeg(media_t& elem, destitem_t& item,
                                   const std::string& extractedpath,
                                   const std::string& convertedpath);
	int          do_convert(media_t& elem, destitem_t& item,
	                        const std::string& extractedpath,
                            const std::string& convertedpath);
	int          do_extract(media_t& elem, destitem_t& item,
	                        const std::string& extractedpath);
	int          do_extract_convert(media_t& elem, destitem_t& item);
	int          check_do_extract_convert(media_t& elem, destitem_t& item);

	void         do_process(media_t& elem);
	void         do_processall();
	void         communicate(const progressdata& commdata);
	
	static void  worker_start(void*);
};

#endif

