#ifndef REMUX_DIALOGPROPERTY_H
#define REMUX_DIALOGPROPERTY_H

class dialogproperty_t {
public:
	void init();
	void show();
	
	int setcurrentelement(int n);

private:
	GtkBuilder *builder;
	GtkDialog  *dialog;
	
	GtkWidget  *label_name;
	GtkWidget  *treeview;
};

#endif

