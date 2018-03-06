#ifndef REMUX_MEDIACONVERT_H
#define REMUX_MEDIACONVERT_H

#include <string>
#include <deque>
#include <thread>
#include <mutex>
#include <atomic>
#include "elements.h"
#include "logfile.h"

class mediaconvert {
public:
	struct progressdata {
		progressdata(media_t* elem)
		 : elem(elem)
		 { }

		media_t    *elem;
	};
	
	struct convertcontext {
	    media_t    *p_elem;
	    int         unique_number;
	    std::string extraction_tmpfile_name;
	    std::string conversion_tmpfile_prefix;
	};
	
	enum exec_state_t {
	    exec_state_run,
	    exec_state_stop
	};
	
	class controller {
	public:
	    explicit controller(mediaconvert *mc);

	    void start_stop();
	    bool completed_stop();
	private:
	    mediaconvert *mc;
	};

	explicit mediaconvert(std::deque<media_t*> c_elementd,
	                      int c_num_processes,
	                      std::string c_ffmpeg_prog,
                          std::string c_mkvextract_prog,
                          std::string c_mkvmerge_prog);
	~mediaconvert();
	
	void start();

	std::deque<progressdata> progressd;
	std::mutex               progressd_lock;
	controller               ctl;
	clslogfile               logfile;

private:
    std::string ffmpeg_prog;
    std::string mkvextract_prog;
    std::string mkvmerge_prog;

    std::deque<media_t*>      m_elementd;
    int                       m_num_processes;
    std::vector<std::thread>  threads;
    std::atomic<exec_state_t> m_state;
    std::atomic<exec_state_t> m_wanted_state;
    int                       unique_number_counter; /* protected by thread_pool_lock */
    std::atomic<int>          m_tasks_count;
    std::mutex                thread_pool_lock;
    
    void worker_start();

	int  do_convert(media_t& elem, destitem_t& item,
	                const std::string& extractedpath,
                    const std::string& convertedpath);

	int  do_extract(media_t& elem, destitem_t& item,
	                const std::string& extractedpath);

	int  do_extract_convert(convertcontext& cvtctx,
	                        destitem_t& item);

	int  check_do_extract_convert(convertcontext& cvtctx,
	                              destitem_t& item);

    int  interlocked_get_context(convertcontext* ctx);
	void process(convertcontext& cvtctx);

	void communicate(const progressdata& commdata);
};

#endif

