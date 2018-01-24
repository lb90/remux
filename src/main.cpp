#include <gtk/gtk.h>
#include "model.h"
#include "scandirectory.h"
#include "window.h"
#include "treeview.h"
#include "app.h"

GtkBuilder *builder;

void cb_app_exit(GSimpleAction*, GVariant*, gpointer userdata) {
	GtkApplication *gtkapp = GTK_APPLICATION(userdata);
	g_application_quit(G_APPLICATION(gtkapp));
}

void cb_startup(GtkApplication* app, gpointer) {
	GtkWidget *window;

	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/form.ui");
	window = GTK_WIDGET(gtk_builder_get_object(builder, "form"));
	
	window_init(window);
	treeview_init(builder);
	
	app_init();
	
	gtk_builder_connect_signals(builder, NULL);
	
	gtk_application_add_window(app, GTK_WINDOW(window));
	
	auto app_exit = g_simple_action_new("exit", NULL);
	g_signal_connect(G_OBJECT(app_exit), "activate", G_CALLBACK(cb_app_exit), (gpointer) app);
	g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(app_exit));
	
	gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
	GtkApplication *gtkapp;
	int status = 0;

	g_set_application_name("Remux");
	gtkapp = gtk_application_new("org.remux.remux", G_APPLICATION_FLAGS_NONE);

#ifdef _WIN32
	GResource *iconsresource = g_resource_load("C:\\icons.gresource", NULL);
	g_resources_register(iconsresource);
#endif
	
	g_signal_connect(G_OBJECT(gtkapp), "startup",  G_CALLBACK(cb_startup),  NULL);
	
	status = g_application_run(G_APPLICATION(gtkapp), argc, argv);
	g_object_unref(gtkapp);

	return status;
}

