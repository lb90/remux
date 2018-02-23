#include <cstdlib>
#include <cassert>
#include <gtk/gtk.h>
#include "app.h"
#include "dialogconversionstart.h"

dialogconversionstart::dialogconversionstart(GtkWindow *window)
{
	builder          = gtk_builder_new_from_resource("/org/remux/remux/ui/conversionstart.ui");
	dialog           = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));

	check_use_timer  = GTK_WIDGET(gtk_builder_get_object(builder, "check_use_timer"));
	label_h          = GTK_WIDGET(gtk_builder_get_object(builder, "label_h"));
	spinbutton_h     = GTK_WIDGET(gtk_builder_get_object(builder, "spinbutton_h"));
	label_m          = GTK_WIDGET(gtk_builder_get_object(builder, "label_m"));
	spinbutton_m     = GTK_WIDGET(gtk_builder_get_object(builder, "spinbutton_m"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
	
	g_signal_connect(check_use_timer, "toggled", cb_toggled_use_timer, (gpointer) this);
	
	timer = false;
	h = 0;
	m = 0;
}

void dialogconversionstart::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}

void dialogconversionstart::() {
	if (dialogstart->timer) {
	    app::set_conversion_timer(dialogstart->h, dialog_start->m);
	}
	else {
	    dialogconversion *dialog = new dialogconversion(window);
        dialog->show();
    }
}

dialogconversionstart::~dialogconversion() {
	g_object_unref(builder);
}

void dialogconversionstart::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogconversionstart *inst = (dialogconversionstart*) self;
	assert(dialog == inst->dialog);
	delete inst;
}

