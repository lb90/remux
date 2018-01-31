#ifndef REMUX_DIALOGCONVERSION_H
#define REMUX_DIALOGCONVERSION_H

class dialogconversion_t {
public:
	explicit
	dialogconversion_t(GtkWindow *window);
	~dialogconversion_t();
	
	void show();

private:
	enum state_t {
		state_ready,
		state_converting,
		state_paused,
		state_stopped,
		state_done
	};
	GtkBuilder *builder;
	GtkDialog  *dialog;
	
	GtkWidget *label;
	GtkWidget *stack;
	GtkWidget *progressbar;
	GtkWidget *image_warning;
	GtkWidget *button_main;
	GtkWidget *button_pause;
	GtkWidget *image_button_start;
	GtkWidget *image_button_pause;
	GtkWidget *image_button_stop;
	GtkWidget *image_button_done;
	
	static
	void self_deleter(GtkDialog *dialog, gpointer self);
};

#endif

