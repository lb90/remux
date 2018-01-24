#ifndef REMUX_LOG_H
#define REMUX_LOG_H

enum log_level_t {
	level_error,
	level_warning,
	level_info,
	level_debug
};

void rmx_log(const char *text, log_level_t level);

#endif

