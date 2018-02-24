#ifndef REMUX_LOGFILE_H
#define REMUX_LOGFILE_H

#include <string>

class clslogfile {
public:
    struct litem_t {
        litem_t()
         : name(),
           startt(time(NULL)),
           endt(startt),
           result(-1),
           text()
         {}

        std::string name;
        time_t      startt;
        time_t      endt;
        int         result;
        std::string text;
    };

    clslogfile();

/* prevent copy/assignment */
    clslogfile(const clslogfile&) = delete;
    void operator=(const clslogfile&) = delete;

    void started(const std::string& name);
    void ended(const std::string& text, int result);

    void save();

private:
    

    std::vector<litem_t> items;
    std::string html;
    
    void write_header();
    void write_info();
    void write_table_description();
    void write_table_row(const litem_t& item);
    void write_footer();
};

#endif


