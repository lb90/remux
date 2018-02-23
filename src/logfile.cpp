#include <cstdlib>
#include <cassert>
#include <ctime>
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <glib.h>
#include "glibutil.h"
#include "app.h"
#include "logfile.h"

clslogfile::clslogfile()
 : items(),
   html()
{
}

std::string util_format_time(time_t t) {
    std::string timefmt;
    tm *lt;
    
    lt = localtime(&t);
    
    timefmt += std::to_string(lt->tm_hour);
    timefmt += ":";
    timefmt += std::to_string(lt->tm_min);
    timefmt += ":";
    timefmt += std::to_string(lt->tm_sec); /*TODO can be 60 for leap seconds */
    
    return timefmt;
}

std::string util_get_duration(time_t t, time_t newt) {
    std::string durfmt;
    
    double elapsedsecs = difftime(t, newt);
    if (int32_t(elapsedsecs) < 0 ||  
        int32_t(elapsedsecs) > std::numeric_limits<int32_t>::max())
        return "";

    int32_t elapsedsecs_aux = int32_t(elapsedsecs);
    int32_t hours = elapsedsecs_aux % 3600;
    elapsedsecs_aux -= hours * 3600;
    int32_t minutes = elapsedsecs_aux % 60;
    elapsedsecs_aux -= minutes * 60;
    int32_t seconds = elapsedsecs_aux;
    
    if (hours < 10) durfmt += "0";
    durfmt += std::to_string(hours);
    durfmt += ":";
    if (minutes < 10) durfmt += "0";
    durfmt += std::to_string(minutes);
    durfmt += ":";
    if (seconds < 10) durfmt += "0";
    durfmt += std::to_string(seconds);
    
    return durfmt;
}

void clslogfile::write_header() {
    std::string header =
"<html lang=\"it\">\n"
"<head>\n"
"<meta charset=\"utf-8\">\n"
"<meta name=\"description\" content=\"Conversion Log\">\n"
"<meta name=\"author\" content=\"Remux\">\n"
"<style media=\"screen\" type=\"text/css\">\n"
"table, th, td {\n"
"border: 1px solid black;\n"
"}\n"
"</style>\n"
"</head>\n"
"<div style=\"overflow-x:auto;\">\n";

    html += header;
}

void clslogfile::write_table_description() {
    std::string tabledesc =
"<table style=\"width:85%\">\n"
"<tr>\n"
"<th>Nome</th>\n"
"<th>Successo</th>\n"
"<th>Warning</th>\n"
"<th>Errore</th>\n"
"<th>Testo</th>\n"
"<th>Inizio</th>\n"
"<th>Durata</th>\n"
"</tr>\n";

    html += tabledesc;
}

void clslogfile::write_table_row(const litem_t& item) {
    std::string strtofmt =
"<tr>\n"
"<td>%s</td>\n"
"<td>%s</td>\n"
"<td>%s</td>\n"
"<td>%s</td>\n"
"<td>%s</td>\n"
"<td>%s</td>\n"
"<td>%s</td>\n"
"</tr>\n";
    const size_t sz = 10000;
    char *cstr = (char*) malloc(sz);
    if (cstr == NULL) return;
    memset(cstr, 0, sz);
    
    std::string name = item.name;
    std::string success;
    std::string warning;
    std::string error;
    if (item.result == 0)
        success = "X";
    else if (item.result == 1)
        warning = "X";
    else if (item.result == 2)
        error = "X";

    std::string text = item.text;
    std::string start = util_format_time(item.startt);
    std::string duration = util_get_duration(item.startt, item.endt);
    int ret =
    snprintf(cstr, sz, strtofmt.c_str(), name.c_str(),
                                         success.c_str(),
                                         warning.c_str(),
                                         error.c_str(),
                                         text.c_str(),
                                         start.c_str(),
                                         duration.c_str());

    std::string str = cstr;
    free(cstr);
    assert(ret >= 0);
    if (size_t(ret) >= sz - 2) return;
    
    html += str;
}

void clslogfile::write_footer() {
    std::string footer =
"</div>\n"
"</html>\n";

    html += footer;
}

void clslogfile::started(const std::string& name) {
    items.emplace_back();
    items.back().name = name;
}

void clslogfile::ended(const std::string& text, int result) {
    items.back().endt = time(NULL);
    items.back().result = result;
    items.back().text = text;
}

void clslogfile::save() {
    std::string basename;
    std::string suffix;
    std::string filename;
    std::string abspath;
    
    suffix = ".html";
    
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    
    std::vector<std::string> months = {
        "gen",
        "feb",
        "mar",
        "apr",
        "mag",
        "giu",
        "lug",
        "ago",
        "set",
        "ott",
        "nov",
        "dic"
    };
    
    basename += std::to_string(lt->tm_year + 1900);
    basename += months.at(lt->tm_mon);
    basename += std::to_string(lt->tm_mday);
    
    for (int c = 0; c < 200; c++) {
        std::string tryfilename;
        std::string tryabspath;
        
        tryfilename = basename;
        if (c > 0)
            tryfilename += "_" + std::to_string(c + 1);
        tryfilename += suffix;
        
        tryabspath = util_build_filename(app::log_dir, tryfilename);
        if (!g_file_test(tryabspath.c_str(), G_FILE_TEST_EXISTS)) {
            filename = tryfilename;
            abspath = tryabspath;
            break;
        }
    }
    
    if (!abspath.empty()) {
        write_header();
        write_table_description();
        
        for (const litem_t& item : items) {
            write_table_row(item);
        }
        
        write_footer();
        
        g_file_set_contents(abspath.c_str(),
                            html.c_str(),
                            -1,
                            NULL);
    }
}
