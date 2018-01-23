#ifndef REMUX_DIALOGPROPERTY_H
#define REMUX_DIALOGPROPERTY_H

#include <gtk/gtk.h>

class dialogproperty_t {
public:
	explicit dialogproperty_t(GtkWindow *window);
	~dialogproperty_t();

	void init();
	void show();
	
	int cancel();
	int setcurrentelement(int n);

private:
	GtkBuilder *builder;
	GtkDialog  *dialog;
	
	GtkWidget  *label_name;
	GtkWidget  *check_ac3ita_aac;
	GtkWidget  *check_intact_ac3;
	GtkWidget  *label_intact_dts;
	GtkWidget  *treeview_item;
	GtkWidget  *entry_outname;
	
	int n;
};

#endif

