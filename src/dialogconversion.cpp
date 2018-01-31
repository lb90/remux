#include <cstdlib>
#include <cassert>
#include <gtk/gtk.h>
#include "dialogconversion.h"
#include "model.h"

dialogconversion_t::dialogconversion_t(GtkWindow *window)
{
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/conversion.ui");
	dialog             = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));

	label              = GTK_WIDGET(gtk_builder_get_object(builder, "label"));
	stack              = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
	progressbar        = GTK_WIDGET(gtk_builder_get_object(builder, "progressbar"));
	button_pause       = GTK_WIDGET(gtk_builder_get_object(builder, "button_pause"));
	button_main        = GTK_WIDGET(gtk_builder_get_object(builder, "button_main"));
	image_button_start = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_start"));
	image_button_pause = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_pause"));
	image_button_stop  = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_stop"));
	image_button_done  = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_done"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
	
	//gtk_button_set_text(
	
	gtk_stack_set_visible_child_name(GTK_STACK(stack), "page_convert");
}

void dialogconversion_t::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}
/*
void dialogconversion_t::setelement(int n) {
	assert(n >= 0);
	assert(size_t(n) < elementv.size());
	
	const media_t& elem = elementv[n];
	
	gtk_label_set_text(GTK_LABEL(label), elem.name.c_str());
}

void dialogconversion_t::setprogress(float fraction) {
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), fraction);
}

void dialogconversion_t::setdone() {
	state = state_done;

	gtk_stack_set_visible_child_name(GTK_STACK(stack), "page_success");
	
	gtk_widget_set_visible(button_pause, FALSE);
	gtk_button_set_text(GTK_BUTTON(button), "_Esci");
	gtk_button_set_image(GTK_BUTTON(button), image_button_done);
	
}

void dialogconversion::dopause() {
}
*/
dialogconversion_t::~dialogconversion_t() {
	g_object_unref(builder);
}

void dialogconversion_t::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogconversion_t *inst = (dialogconversion_t*) self;
	assert(dialog == inst->dialog);
	delete inst;
}

