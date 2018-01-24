#ifndef REMUX_DIALOGSETTINGS_H
#define REMUX_DIALOGSETTINGS_H

#include <gtk/gtk.h>

class dialogsettings_t {
public:
	explicit dialogsettings_t(GtkWindow *window);
	~dialogsettings_t();

	void show();
	
private:
	GtkBuilder *builder;
	GtkDialog  *dialog;
	
	static
	void self_deleter(GtkDialog *dialog, gpointer self);
};

#endif

