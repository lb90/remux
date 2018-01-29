#ifndef REMUX_APP_H
#define REMUX_APP_H

class app {
public:
	static int init();

	static void scandirectory(const char *directoryname);
	static void elementactivated(int n);
};

#endif
