#include <cstdlib>
#include "dialogproperty.h"

void dialogproperty::init() {
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/property.ui");
	
	dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog_property"));
	
	label_name = GTK_WIDGET(gtk_builder_get_object(builder, "label_name"));
}
