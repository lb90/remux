#include <cstdlib>
#include <cassert>
#include "dialogproperty.h"
#include "model.h"
#include "elements.h"

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
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
}

void dialogproperty_t::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}

int dialogproperty_t::setcurrentelement(gint n) {
	assert(n >= 0);
	assert(size_t(n) < elementv.size());
	
	if (cur == n)
		return 0;
	else
		cancel();

	media_t& media = elementv[n];
	
	gtk_label_set_text(GTK_LABEL(label_name), media.name.c_str());
	basic_model = basic_list_model_new(media.itemv.size());
	
	/*TODO
	cell data functions for cell renderers
	*/
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_item), GTK_TREE_MODEL(basic_model));
	
	return 0;
}

int dialogproperty_t::cancel(gint n) {
	return 0;
}

dialogproperty_t::~dialogproperty_t() {
	g_object_unref(builder);
}

void dialogproperty_t::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	assert(dialog == inst->dialog);
	delete inst;
}
