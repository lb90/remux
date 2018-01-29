#ifndef REMUX_LAUNCHPROCESS_H
#define REMUX_LAUNCHPROCESS_H

#include <vector>
#include <string>

int launch_process(const std::vector<std::string>& argv, std::string& outputstring, bool want_stdout = false);

#endif

