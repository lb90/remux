#ifndef REMUX_LOGFILE_H
#define REMUX_LOGFILE_H

class clslogfile {
public:
    clslogfile();
    ~clslogfile();
/* prevent copy/assignment */
    clslogfile(const clslogfile&) = delete;
    void operator=(const clslogfile&) = delete;

    void write_highlight(const std::string&);
    void write_normal(const std::string&);
    void write_bold(const std::string&);

    void save();

private:
    std::vector<std::string> items;
};

#endif

