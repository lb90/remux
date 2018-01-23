#include <cstdlib>
#include "dialogproperty.h"

dialogproperty_t::dialogproperty_t(GtkWindow *window)
{
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/property.ui");
	
	dialog           = GTK_DIALOG(gtk_builder_get_object(builder, "dialog_property"));
	label_name       = GTK_WIDGET(gtk_builder_get_object(builder, "label_name"));
	check_ac3ita_aac = GTK_WIDGET(gtk_builder_get_object(builder, "check_ac3ita_aac"));
	check_intact_ac3 = GTK_WIDGET(gtk_builder_get_object(builder, "check_intact_ac3"));
	label_intact_dts = GTK_WIDGET(gtk_builder_get_object(builder, "label_intact_dts"));
	treeview_item    = GTK_WIDGET(gtk_builder_get_object(builder, "treeview_item"));
	entry_outname    = GTK_WIDGET(gtk_builder_get_object(builder, "entry_outname"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
}

void dialogproperty_t::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}

dialogproperty_t::~dialogproperty_t() {
	g_object_unref(builder);
}
