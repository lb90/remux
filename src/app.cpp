#include <cstdlib>
#include "model.h"
#include "scandirectory.h"

void app_scandirectory(const char *directoryname) {
	model_clear();
	scan_directory(directoryname);
}

int app_init() {
	//connect to our signals
	return 0;
}

