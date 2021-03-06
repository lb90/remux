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
	
	
	GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
	GtkTextIter enditer;
	gtk_text_buffer_get_end_iter(textbuffer, &enditer);
	endmark = gtk_text_buffer_create_mark(textbuffer, NULL, &enditer, FALSE); /* switch-to-right mark */
	gtk_text_buffer_create_tag(textbuffer, "tag-bold", "weight", PANGO_WEIGHT_BOLD, NULL);
	gtk_text_buffer_create_tag(textbuffer, "tag-red", "foreground", "#ff0000", NULL);
}

void dialogconversion::show() {
	mc = new mediaconvert();
	g_timeout_add(100, dialogconversion::check_do_communication, (gpointer) this);

	gtk_window_present(GTK_WINDOW(dialog));
}

void dialogconversion::append_to_textview(const std::string& text, bool bold, bool red) {
	GtkTextBuffer *textbuffer;
	GtkTextIter    iter;
	textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
	gtk_text_buffer_get_end_iter(textbuffer, &iter);
	if (!bold && !red)
		gtk_text_buffer_insert(textbuffer, &iter, text.c_str(), -1);
	else if (bold && red)
		gtk_text_buffer_insert_with_tags_by_name(textbuffer, &iter, text.c_str(), -1, "tag-bold", "tag-red", NULL);
	else if (bold) {
		gtk_text_buffer_insert_with_tags_by_name(textbuffer, &iter, text.c_str(), -1, "tag-bold", NULL);
	}
	else if (red) {
		gtk_text_buffer_insert_with_tags_by_name(textbuffer, &iter, text.c_str(), -1, "tag-red", NULL);
	}
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(textview), endmark, 0.0f, FALSE, 0.0f, 0.0f);
}

int dialogconversion::check_do_communication(gpointer self) {
	dialogconversion *inst = (dialogconversion*) self;
	std::lock_guard<std::mutex> lock(inst->mc->progressd_lock);
	
	if (inst->mc->progressd.empty())
		return TRUE;
	
	for (const auto& commdata : inst->mc->progressd) {
		if (commdata.total != 0) {
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(inst->progressbar), float(commdata.n) / float(commdata.total));
		}
		if (commdata.elem != nullptr) {
			gtk_label_set_text(GTK_LABEL(inst->label), commdata.elem->name.c_str());
			inst->append_to_textview("\n" + commdata.elem->name, true, true);
		}
		if (!commdata.text.empty())
			inst->append_to_textview("\n" + commdata.text);
	}
	
	auto& commdata = inst->mc->progressd.back();

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
	
	append_to_textview("\nFine", true, false);
}

void dialogconversion::cb_want_close(GtkButton *, gpointer self) {
	dialogconversion *dialog = (dialogconversion*) self;
	
	if (dialog->state == state_end) {
		gtk_widget_destroy(GTK_WIDGET(dialog->dialog));
	}
}

void dialogconversion::cb_want_pause(GtkButton *, gpointer self) {
}

dialogconversion::~dialogconversion() {
	if (mc) {
		/* signal want to end */
		/* mc->cleanup(); */
		mc->worker.join();
		delete mc;
		mc = nullptr;
	}

	g_object_unref(builder);
}

void dialogconversion::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogconversion *inst = (dialogconversion*) self;
	assert(dialog == inst->dialog);
	delete inst;
}

