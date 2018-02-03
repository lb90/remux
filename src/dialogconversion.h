#ifndef REMUX_DIALOGCONVERSION_H
#define REMUX_DIALOGCONVERSION_H

#include <gtk/gtk.h>
#include "mediaconvert.h"

class dialogconversion {
public:
	explicit
	dialogconversion(GtkWindow *window);
	~dialogconversion();
	
	void show();

private:
	enum state_t {
		state_converting,
		state_paused,
		state_end,
	};
	
	GtkBuilder *builder;
	GtkDialog  *dialog;
	
	GtkWidget *label;
	GtkWidget *stack;
	GtkWidget *progressbar;
	GtkWidget *image_warning;
	GtkWidget *button_close;
	GtkWidget *button_pause;
	GtkWidget *image_button_start;
	GtkWidget *image_button_pause;
	GtkWidget *image_button_stop;
	GtkWidget *image_button_done;
	GtkWidget *textview;
	
	state_t       state;
	
	mediaconvert *mc;
	
	static int  check_do_communication(void *);

	static void cb_want_close(GtkButton *, gpointer self);
	static void cb_want_pause(GtkButton *, gpointer self);
	
	void done();
	
	static
	void self_deleter(GtkDialog *dialog, gpointer self);
};

#endif

