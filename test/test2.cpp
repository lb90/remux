#include <cstdlib>
#include <gtk/gtk.h>

GtkWindow *window;

int main(int argc, char **argv) {
	gtk_init(&argc, &argv);

	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	g_signal_connect(window, "destroy", gtk_main_quit, NULL);

	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();

	return 0;
}

