#include <cstdlib>
#include "dialogconv.h"

void dialogconv::init() {
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/conversion.ui");
	
	dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog_conv"));
	
	label_name = GTK_WIDGET(gtk_builder_get_object(builder, "label_name"));
}
