#include <cstdlib>
#include <cassert>
#include <climits>
#include <gtk/gtk.h>
#include "dialogsettings.h"

dialogsettings_t::dialogsettings_t(GtkWindow *window)
{
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/settings.ui");
	
	dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog_settings"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
}

void dialogsettings_t::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}

void dialogsettings_t::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogsettings_t *inst = (dialogsettings_t*) self;
	assert(dialog == inst->dialog);
	delete inst;
}

dialogsettings_t::~dialogsettings_t() {
	g_object_unref(builder);
}

