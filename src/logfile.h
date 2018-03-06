#ifndef REMUX_LOGFILE_H
#define REMUX_LOGFILE_H

#include <ctime>
#include <vector>
#include <string>
#include <mutex>

class clslogfile {
public:
    struct litem_t {
        litem_t()
         : name(),
           startt(),
           endt(),
           text(),
           result(-1)
         {}

        std::string name;
        time_t      startt;
        time_t      endt;
        std::string text;
        int         result;
    };

    clslogfile();

/* prevent copy/assignment */
    clslogfile(const clslogfile&) = delete;
    void operator=(const clslogfile&) = delete;
    
    void publish(time_t t1,
                 time_t t2,
                 const std::string& name,
                 const std::string& text,
                 int result);

    void save();

private:
    std::vector<litem_t> items;
    std::mutex  m_mutex;
    std::string html;
    
    void write_header();
    void write_info();
    void write_table_description();
    void write_table_row(const litem_t& item);
    void write_footer();
};

#endif


