#ifndef REMUX_PARSEINFO_H
#define REMUX_PARSEINFO_H

#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>

int parse_info(std::stringstream& info,
              boost::property_tree::ptree& pt);

#endif
