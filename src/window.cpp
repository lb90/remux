#include <cstdlib>
#include <cassert>
#include <climits>
#include <gtk/gtk.h>
#include "config.h"
#include "window.h"
#include "signalcentre.h"
#include "treeview.h"
#include "dialogsettings.h"
#include "dialogconversion.h"
#include "app.h"
#include "op.h"

void cb_win_open(GSimpleAction*, GVariant*, gpointer userdata) {
	GtkFileChooserNative *native;
	gint res = 0;
	GtkWindow *window = GTK_WINDOW(userdata);
	
	native = gtk_file_chooser_native_new("Apri Media",
                                         window,
                                         GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                         "_Apri",
                                         "_Annulla");
	res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
	if (res == GTK_RESPONSE_ACCEPT) {
		char *directoryname;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
		directoryname = gtk_file_chooser_get_filename(chooser);
		
		app::scandirectory(directoryname);
		
		g_free(directoryname);
	}

	g_object_unref (native);
}

void cb_win_selall(GSimpleAction*, GVariant*, gpointer userdata) {
	treeview_select(select_all);
}

void cb_win_selnone(GSimpleAction*, GVariant*, gpointer userdata) {
	treeview_select(select_none);
}

void cb_win_selinvert(GSimpleAction*, GVariant*, gpointer userdata) {
	treeview_select(select_invert);
}

void cb_win_remove(GSimpleAction*, GVariant*, gpointer userdata) {
	std::vector<size_t> indexv;
	treeview_getselection(indexv);
	op::remove(indexv);
}

void cb_win_convert(GSimpleAction*, GVariant*, gpointer userdata) {
	GtkWindow *window = GTK_WINDOW(userdata);

	dialogconversion_t *dialog = new dialogconversion_t(window);
	dialog->show();
}

void cb_win_showproperty(GSimpleAction*, GVariant*, gpointer userdata) {
	std::vector<size_t> indexv;

	treeview_getselection(indexv);

	if (indexv.empty())
		return;
	if (indexv.size() == 1) {
		size_t i = indexv[0];
		assert(i < INT_MAX);
		signalcentre::emit("element\\activate", int(i));
	}
	else {
		/*TODO*/
	}
}

void cb_win_showsettings(GSimpleAction*, GVariant*, gpointer userdata) {
	GtkWindow *window = GTK_WINDOW(userdata);

	dialogsettings_t *dialog = new dialogsettings_t(window);
	dialog->show();
}

void cb_win_showabout(GSimpleAction*, GVariant*, gpointer userdata) {
	GtkWidget *window = (GtkWidget *) userdata;
	
	gtk_show_about_dialog(GTK_WINDOW(window),
	                      "program-name", "Remux",
	                      "version",      REMUX_VERSION,
	                      "website",      "github.com/lb90",
	                      NULL);
}

GtkWindow *get_window(GtkWindow *registerw) {
	static GtkWindow *win = NULL;
	
	if (registerw)
		win = registerw;
	
	return win;
}

int window_init(GtkWidget *window) {
	GActionEntry action_entries[] = {
		{"open",           cb_win_open,           NULL, NULL, NULL },
		{"convert",        cb_win_convert,        NULL, NULL, NULL },
		{"remove",         cb_win_remove,         NULL, NULL, NULL },
		{"selectall",      cb_win_selall,         NULL, NULL, NULL },
		{"selectnone",     cb_win_selnone,        NULL, NULL, NULL },
		{"selectinvert",   cb_win_selinvert,      NULL, NULL, NULL },
		{"showproperty",   cb_win_showproperty,   NULL, NULL, NULL },
		{"showsettings",   cb_win_showsettings,   NULL, NULL, NULL },
		{"showabout",      cb_win_showabout,      NULL, NULL, NULL },
	};
	g_action_map_add_action_entries(G_ACTION_MAP(window),
	                                action_entries,
	                                G_N_ELEMENTS(action_entries),
	                                window);
	get_window(GTK_WINDOW(window));
	return 0;
}

