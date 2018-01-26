#include <cstdlib>
#include <cassert>
#include <gtk/gtk.h>
#include "dialogconversion.h"
#include "model.h"

dialogconversion_t::dialogconversion_t(GtkWindow *window)
{
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/conversion.ui");
	dialog           = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));

	label            = GTK_WIDGET(gtk_builder_get_object(builder, "label"));
	stack            = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
	progressbar      = GTK_WIDGET(gtk_builder_get_object(builder, "progressbar"));
	button_pause     = GTK_WIDGET(gtk_builder_get_object(builder, "button_pause"));
	button           = GTK_WIDGET(gtk_builder_get_object(builder, "button"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
}

void dialogconversion_t::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}

dialogconversion_t::~dialogconversion_t() {
	g_object_unref(builder);
}

void dialogconversion_t::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogconversion_t *inst = (dialogconversion_t*) self;
	assert(dialog == inst->dialog);
	delete inst;
}

