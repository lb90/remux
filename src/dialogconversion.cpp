#include <cstdlib>
#include <cassert>
#include <gtk/gtk.h>
#include "dialogconversion.h"
#include "model.h"

dialogconversion::dialogconversion(GtkWindow *window)
{
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/conversion.ui");
	dialog             = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));

	label              = GTK_WIDGET(gtk_builder_get_object(builder, "label"));
	stack              = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
	progressbar        = GTK_WIDGET(gtk_builder_get_object(builder, "progressbar"));
	button_pause       = GTK_WIDGET(gtk_builder_get_object(builder, "button_pause"));
	button_close       = GTK_WIDGET(gtk_builder_get_object(builder, "button_close"));
	image_button_start = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_start"));
	image_button_pause = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_pause"));
	image_button_stop  = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_stop"));
	image_button_done  = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_done"));
	textview           = GTK_WIDGET(gtk_builder_get_object(builder, "textview"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(dialogconversion::self_deleter), (gpointer) this);
	
	g_signal_connect(button_close, "clicked", G_CALLBACK(dialogconversion::cb_want_close), (gpointer) this);
	g_signal_connect(button_pause, "clicked", G_CALLBACK(dialogconversion::cb_want_pause), (gpointer) this);
	
	gtk_stack_set_visible_child_name(GTK_STACK(stack), "page_convert");
}

void dialogconversion::show() {
	mc = new mediaconvert();
	g_timeout_add(500, dialogconversion::check_do_communication, (gpointer) this);

	gtk_window_present(GTK_WINDOW(dialog));
}

int dialogconversion::check_do_communication(gpointer self) {
	dialogconversion *inst = (dialogconversion*) self;
	std::lock_guard<std::mutex> lock(inst->mc->progressd_lock);
	
	if (inst->mc->progressd.empty())
		return TRUE;
	
	for (const auto& commdata : inst->mc->progressd) {
		if (commdata.elem) {
			GtkTextBuffer *textbuffer;
			GtkTextIter    iter;
			textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(inst->textview));
			gtk_text_buffer_get_end_iter(textbuffer, &iter);
			gtk_text_buffer_insert(textbuffer, &iter, ("\n" + commdata.elem->name).c_str(), -1);
		}
		if (!commdata.text.empty()) {
			GtkTextBuffer *textbuffer;
			GtkTextIter    iter;
			textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(inst->textview));
			gtk_text_buffer_get_end_iter(textbuffer, &iter);
			gtk_text_buffer_insert(textbuffer, &iter, ("\n" + commdata.text).c_str(), -1);
		}
	}
	
	auto& commdata = inst->mc->progressd.back();
	
	if (commdata.elem) {
		gtk_label_set_text(GTK_LABEL(inst->label), commdata.elem->name.c_str());
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(inst->progressbar), float(commdata.n) / float(commdata.total));
	}
	if (commdata.done) {
		inst->done();
		return FALSE;
	}
	
	inst->mc->progressd.clear();
	
	return TRUE;
}

void dialogconversion::done() {
	gtk_label_set_text(GTK_LABEL(label), "Fine");
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), 1.0f);
	
	gtk_widget_set_sensitive(button_pause, FALSE);
	
	gtk_stack_set_visible_child_name(GTK_STACK(stack), "page_done");
	
	gtk_button_set_label(GTK_BUTTON(button_close), "_Chiudi");
	gtk_button_set_image(GTK_BUTTON(button_close), image_button_done);
	
	state = state_end;
}

void dialogconversion::cb_want_close(GtkButton *, gpointer self) {
	dialogconversion *dialog = (dialogconversion*) self;
	
	if (dialog->state == state_end) {
		gtk_widget_destroy(GTK_WIDGET(dialog->dialog));
	}
}

void dialogconversion::cb_want_pause(GtkButton *, gpointer self) {
}

/*
void dialogconversion::setelement(int n) {
	assert(n >= 0);
	assert(size_t(n) < elementv.size());
	
	const media_t& elem = elementv[n];
	
	gtk_label_set_text(GTK_LABEL(label), elem.name.c_str());
}

void dialogconversion::setprogress(float fraction) {
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
dialogconversion::~dialogconversion() {
	g_object_unref(builder);
}

void dialogconversion::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogconversion *inst = (dialogconversion*) self;
	assert(dialog == inst->dialog);
	delete inst;
}

