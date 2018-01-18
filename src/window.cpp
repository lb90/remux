#include <cstdlib>
#include <gtk/gtk.h>
#include "window.h"
#include "app.h"

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
		
		app_scandirectory(directoryname);
		
		g_free(directoryname);
	}

	g_object_unref (native);
}

void cb_win_convert(GSimpleAction*, GVariant*, gpointer userdata) {
}

void cb_win_remove(GSimpleAction*, GVariant*, gpointer userdata) {
}

void cb_win_showproperties(GSimpleAction*, GVariant*, gpointer userdata) {
}

void cb_win_showabout(GSimpleAction*, GVariant*, gpointer userdata) {
}

int window_init(GtkWidget *window) {
	GActionEntry action_entries[] = {
		{"open",           cb_win_open,           NULL, NULL, NULL },
		{"convert",        cb_win_convert,        NULL, NULL, NULL },
		{"remove",         cb_win_remove,         NULL, NULL, NULL },
		{"showproperties", cb_win_showproperties, NULL, NULL, NULL },
		{"showabout",      cb_win_showabout,      NULL, NULL, NULL },
	};
	g_action_map_add_action_entries(G_ACTION_MAP(window),
	                                action_entries,
	                                G_N_ELEMENTS(action_entries),
	                                window);

	return 0;
}
