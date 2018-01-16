#include <gtk/gtk.h>
#include "model.h"
#include "directory.h"
#include "treeview.h"

GtkBuilder *builder;
GtkWidget  *window;

GtkApplication *gtkapp;

void cb_app_open(GSimpleAction*, GVariant*, gpointer) {
	GtkFileChooserNative *native;
	gint res = 0;
	
	native = gtk_file_chooser_native_new("Apri Media",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                         "_Apri",
                                         "_Annulla");
	res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
	if (res == GTK_RESPONSE_ACCEPT) {
		char *directoryname;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
		directoryname = gtk_file_chooser_get_filename(chooser);
		
		model_clear();
		scan_directory(directoryname);
		
		g_free(directoryname);
	}

	g_object_unref (native);
}

void cb_app_apply(GSimpleAction*, GVariant*, gpointer) {
}

void cb_app_exit(GSimpleAction*, GVariant*, gpointer) {
	g_application_quit(G_APPLICATION(gtkapp));
}

int setup() {
	builder = gtk_builder_new_from_resource("/ui/form.ui");
	window = GTK_WIDGET(gtk_builder_get_object(builder, "form"));
	
	treeview_init(builder);
	
	return 0;
}

void cb_startup(GtkApplication* app, gpointer) {
	setup();
	
	gtk_application_add_window(app, GTK_WINDOW(window));
	
	auto app_open = g_simple_action_new("open", NULL);
	g_signal_connect(G_OBJECT(app_open), "activate", G_CALLBACK(cb_app_open), NULL);
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(app_open));
	
	auto app_apply = g_simple_action_new("apply", NULL);
	g_signal_connect(G_OBJECT(app_apply), "activate", G_CALLBACK(cb_app_apply), NULL);
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(app_apply));
	
	auto app_exit = g_simple_action_new("exit", NULL);
	g_signal_connect(G_OBJECT(app_exit), "activate", G_CALLBACK(cb_app_exit), NULL);
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(app_exit));
	
	gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
	int status = 0;

	g_set_application_name("Remux");
	gtkapp = gtk_application_new(NULL, G_APPLICATION_FLAGS_NONE);
	
	g_signal_connect(G_OBJECT(gtkapp), "startup",  G_CALLBACK(cb_startup),  NULL);
	
	status = g_application_run(G_APPLICATION(gtkapp), argc, argv);
	g_object_unref(gtkapp);

	return status;
}

