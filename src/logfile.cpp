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
    
    double elapsedsecs = difftime(newt, t);
    if (int32_t(elapsedsecs) < 0 ||  
        int32_t(elapsedsecs) > std::numeric_limits<int32_t>::max())
        return "";

    int32_t elapsedsecs_aux = int32_t(elapsedsecs);
    int32_t seconds = elapsedsecs_aux % 60;
    elapsedsecs_aux -= seconds;
    int32_t minutes = (elapsedsecs_aux % 3600) / 60;
    elapsedsecs_aux -= minutes * 60;
    int32_t hours = elapsedsecs_aux / 3600;
    
    if (hours > 0) {
        if (hours < 10) durfmt += "0";
        durfmt += std::to_string(hours);
        durfmt += "h ";
    }
    if (minutes > 0) {
        if (minutes < 10) durfmt += "0";
        durfmt += std::to_string(minutes);
        durfmt += "m ";
    }
    if (seconds < 10) durfmt += "0";
    durfmt += std::to_string(seconds);
    durfmt += "s";
    
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
"border-collapse: collapse;"
"}\n"
"tr:nth-child(even) {\n"
"background-color: #dddddd;\n"
"}\n"
"</style>\n"
"</head>\n"
"<div style=\"overflow-x:auto;\">\n";

    html += header;
}

void clslogfile::write_info() {
    std::vector<std::string> days = {
        "Domenica",
        "Lunedì",
        "Martedì",
        "Mercoledì",
        "Giovedì",
        "Venerdì",
        "Sabato"
    };
    std::vector<std::string> months = {
        "Gennaio",
        "Febbraio",
        "Marzo",
        "Aprile",
        "Maggio",
        "Giugno",
        "Luglio",
        "Agosto",
        "Settembre",
        "Ottobre",
        "Novembre",
        "Dicembre"
    };
    
    int c_success = 0;
    int c_warning = 0;
    int c_error = 0;
    for (const litem_t item : items) {
        if (item.result == 0)
            c_success++;
        else if (item.result == 1)
            /*c_warning++;*/c_success++;
        else if (item.result == 2)
            c_error++;
    }

    std::string infotofmt =
"<h1>%s %s %s %s</h1>\n"
"<div>\n"
"&#9989 Totale successi: %s<br>\n"
/*"&#9888 Totale warning: %s\n"*/
"&#10062 Totale errori: %s\n"
"</div>\n";

    const size_t sz = 10000;
    char *cstr = (char*) malloc(sz);
    if (cstr == NULL) return;
    memset(cstr, 0, sz);
    
    time_t t = items.front().startt;
    struct tm *lt = localtime(&t);
    
    std::string day = days.at(lt->tm_wday);
    std::string daynumber = std::to_string(lt->tm_mday);
    std::string month = months.at(lt->tm_mon);
    std::string year = std::to_string(lt->tm_year + 1900);
    std::string success_count = std::to_string(c_success) + "/" + std::to_string(items.size());
    std::string warning_count = std::to_string(c_warning) + "/" + std::to_string(items.size());
    std::string error_count = std::to_string(c_error) + "/" + std::to_string(items.size());

    std::string info;
    int ret =
    snprintf(cstr, sz, infotofmt.c_str(), day.c_str(),
                                          daynumber.c_str(),
                                          month.c_str(),
                                          year.c_str(),
                                          success_count.c_str(),
                                          warning_count.c_str(),
                                          error_count.c_str());

    info = cstr;
    free(cstr);
    assert(ret >= 0);
    if (size_t(ret) >= sz - 2) return;
    
    html += info;
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
        success = "&#9989";
    else if (item.result == 1)
        warning = "&#9888";
    else if (item.result == 2)
        error = "&#10062";

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
    
    if (items.empty()) return;
    
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
        write_info();
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
