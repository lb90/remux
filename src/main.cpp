#include <gtk/gtk.h>
#include "model.h"
#include "settings.h"
#include "scandirectory.h"
#include "window.h"
#include "treeview.h"
#include "app.h"
#include "glibutil.h"
#include "dialogproperty.h"

GtkBuilder *builder;

void cb_app_exit(GSimpleAction*, GVariant*, gpointer userdata) {
	GtkApplication *gtkapp = GTK_APPLICATION(userdata);
	g_application_quit(G_APPLICATION(gtkapp));
}

void cb_app_enqueue(GSimpleAction*, GVariant*, gpointer userdata) {
	if (app::dialogproperty) {
		dialogproperty_t::cb_enqueue(NULL, app::dialogproperty);
	}
}

void cb_activate(GtkApplication* app, gpointer) {
	if (get_window(NULL))
		gtk_window_present(get_window(NULL));
}

void cb_startup(GtkApplication* gtkapp, gpointer) {
	GtkWidget *window;
	
	settings::init();
	
	auto app_exit = g_simple_action_new("exit", NULL);
	g_signal_connect(G_OBJECT(app_exit), "activate", G_CALLBACK(cb_app_exit), (gpointer) gtkapp);
	g_action_map_add_action(G_ACTION_MAP(gtkapp), G_ACTION(app_exit));
	auto app_enqueue = g_simple_action_new("enqueue", NULL);
	g_signal_connect(G_OBJECT(app_enqueue), "activate", G_CALLBACK(cb_app_enqueue), (gpointer) gtkapp);
	g_action_map_add_action(G_ACTION_MAP(gtkapp), G_ACTION(app_enqueue));

	const char* accels[] = {NULL, NULL};
	accels[0] = "<Control>A";
	gtk_application_set_accels_for_action(gtkapp, "app.enqueue", accels);

	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/form.ui");
	window = GTK_WIDGET(gtk_builder_get_object(builder, "form"));
	gtk_application_add_window(gtkapp, GTK_WINDOW(window));
	
	window_init(window, gtkapp);
	treeview_init(builder);
	
	app::init();
	app::gtkapp = gtkapp;
	
	gtk_builder_connect_signals(builder, NULL);
	
	gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
	GtkApplication *gtkapp;
	int status = 0;

	#ifdef _WIN32
	/*GResource *iconsresource = g_resource_load(util_filename_from_install_directory("icons.gresource").c_str(), NULL);
	g_resources_register(iconsresource);*/
	#endif
	#ifdef _WIN32
	g_setenv("GTK_CSD", "1", FALSE);
	#endif

	g_set_application_name("Remux");
	gtkapp = gtk_application_new("org.remux.remux", G_APPLICATION_NON_UNIQUE);
	
	g_signal_connect(G_OBJECT(gtkapp), "startup",  G_CALLBACK(cb_startup),  NULL);
	g_signal_connect(G_OBJECT(gtkapp), "activate",  G_CALLBACK(cb_activate),  NULL);
	
	status = g_application_run(G_APPLICATION(gtkapp), argc, argv);
	g_object_unref(gtkapp);

	return status;
}

