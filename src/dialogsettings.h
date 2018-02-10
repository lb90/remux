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
	
	GtkFileChooser  *chooser_mkvtoolnix;
	GtkFileChooser  *chooser_ffmpeg;
	
	static
	void response(GtkDialog *dialog, gint resp_id, gpointer self);
	
	static
	void self_deleter(GtkDialog *dialog, gpointer self);
};

#endif

