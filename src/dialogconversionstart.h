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

private:
	GtkBuilder  *builder;
	GtkDialog   *dialog;
	
	GtkWidget   *check_use_timer;
	GtkWidget   *label_h;
	GtkWidget   *spinbutton_h;
	GtkWidget   *label_m;
	GtkWidget   *spinbutton_m;
	
	GtkWidget   *button_ok;
	GtkWidget   *button_close;
	
	static
	void cb_toggled_use_timer(GtkToggleButton *toggle, void *self);
	
    static
	void cb_ok(GtkButton *button, void *self);
	
	static
	void cb_close(GtkButton *button, void *self);
	
	static
	void self_deleter(GtkDialog *dialog, gpointer self);
};

#endif

