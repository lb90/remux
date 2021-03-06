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
		progressdata(int total,
		             int n,
		             media_t* elem,
		             const std::string& text,
		             bool done = false)
		 : total(total),
		   n(n),
		   elem(elem),
		   text(text),
		   done(done)
		 { }

		int         total;
		int         n;
		media_t    *elem;
		std::string text;
		bool        done;
	};

	explicit     mediaconvert();
	void         start();

	std::deque<progressdata> progressd;
	std::mutex               progressd_lock;
	std::thread              worker;
	
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

