#ifndef REMUX_DIALOGCONVERSIONSTART_H
#define REMUX_DIALOGCONVERSIONSTART_H

#include <gtk/gtk.h>
#include "mediaconvert.h"

class dialogconversionstart {
public:
	explicit
	dialogconversionstart(GtkWindow *window);
	~dialogconversionstart();
	
	void show();
	
	bool timer;
	int hour;
	int minute;

private:
	GtkBuilder  *builder;
	GtkDialog   *dialog;
	
	GtkWidget   *check_use_timer;
	GtkWidget   *label_h;
	GtkWidget   *spinbutton_h;
	GtkWidget   *label_m;
	GtkWidget   *spinbutton_m;
};

#endif

