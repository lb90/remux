#ifndef REMUX_DIALOGCONVERSION_H
#define REMUX_DIALOGCONVERSION_H

#include <memory>
#include <gtk/gtk.h>
#include "mediaconvert.h"

class dialogconversion {
public:
	explicit
	dialogconversion(GtkWindow *window);
	~dialogconversion();
	
	void show();

private:
	enum format_t {
		format_none,
		format_bold,
		format_bold_red
	};
	
	enum state_t {
	    state_converting,
	    state_pausing,
	    state_done
	};
	
	GtkBuilder  *builder;
	GtkDialog   *dialog;
	
	GtkWidget   *label;
	GtkWidget   *stack;
	GtkWidget   *progressbar;
	GtkWidget   *image_warning;
	GtkWidget   *button_stop;
	GtkWidget   *button_pause;
	GtkWidget   *image_button_start;
	GtkWidget   *image_button_pause;
	GtkWidget   *image_button_stop;
	GtkWidget   *image_button_done;
	GtkWidget   *textview;
	GtkTextMark *endmark;
	guint        check_source;
	
	std::unique_ptr<mediaconvert> mc;
	state_t      state;
	int          num_total;
	int          num_completed;
	
	void append_to_textview(const std::string& text, bool bold = false,
	                                                 bool red = false);
	
	static int  check_do_communication(void *);
	static int  check_mc_has_paused(void *);

	static void cb_button_stop(GtkButton *, gpointer self);
	static void cb_button_pause(GtkButton *, gpointer self);
	
	void done();
	
	static
	void self_deleter(GtkDialog *dialog, gpointer self);
};

#endif

