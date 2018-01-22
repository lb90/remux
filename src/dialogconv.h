#ifndef REMUX_DIALOGCONV_H
#define REMUX_DIALOGCONV_H

class dialogconversion {
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

