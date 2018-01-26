#ifndef REMUX_DIALOGCONVERSION_H
#define REMUX_DIALOGCONVERSION_H

class dialogconversion_t {
public:
	explicit
	dialogconversion_t(GtkWindow *window);
	~dialogconversion_t();
	
	void show();

private:
	GtkBuilder *builder;
	GtkDialog  *dialog;
	
	GtkWidget *label;
	GtkWidget *stack;
	GtkWidget *progressbar;
	GtkWidget *image_warning;
	GtkWidget *button;
	GtkWidget *button_pause;
	
	static
	void self_deleter(GtkDialog *dialog, gpointer self);
};

#endif

