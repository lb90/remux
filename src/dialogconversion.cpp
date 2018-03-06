#include <cstdlib>
#include <cassert>
#include <gtk/gtk.h>
#include <boost/numeric/conversion/cast.hpp>
#include "dialogconversion.h"
#include "model.h"
#include "app.h"

dialogconversion::dialogconversion(GtkWindow *window)
{
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/conversion.ui");
	dialog             = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));

	label              = GTK_WIDGET(gtk_builder_get_object(builder, "label"));
	stack              = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
	progressbar        = GTK_WIDGET(gtk_builder_get_object(builder, "progressbar"));
	button_stop        = GTK_WIDGET(gtk_builder_get_object(builder, "button_stop"));
	button_pause       = GTK_WIDGET(gtk_builder_get_object(builder, "button_pause"));
	image_button_start = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_start"));
	image_button_pause = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_pause"));
	image_button_stop  = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_stop"));
	image_button_done  = GTK_WIDGET(gtk_builder_get_object(builder, "image_button_done"));
	textview           = GTK_WIDGET(gtk_builder_get_object(builder, "textview"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(dialogconversion::self_deleter), (gpointer) this);
	
	g_signal_connect(button_stop, "clicked", G_CALLBACK(dialogconversion::cb_button_stop), (gpointer) this);
	g_signal_connect(button_pause, "clicked", G_CALLBACK(dialogconversion::cb_button_pause), (gpointer) this);
	
	gtk_stack_set_visible_child_name(GTK_STACK(stack), "page_convert");
	
	GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
	GtkTextIter enditer;
	gtk_text_buffer_get_end_iter(textbuffer, &enditer);
	endmark = gtk_text_buffer_create_mark(textbuffer, NULL, &enditer, FALSE); /* switch-to-right mark */
	gtk_text_buffer_create_tag(textbuffer, "tag-bold", "weight", PANGO_WEIGHT_BOLD, NULL);
	gtk_text_buffer_create_tag(textbuffer, "tag-red", "foreground", "#ff0000", NULL);
	
	num_total = 0;
	num_completed = 0;
	check_source = 0;
}

void dialogconversion::show() {
    std::deque<media_t*> elementd;
    
    for (auto& elem : elementv) {
        if (elem.isready)
            elementd.emplace_back(&elem);
    }
    
    num_total = boost::numeric_cast<int>(elementd.size());
    num_completed = 0;
    state = state_converting;
    
	mc.reset(new mediaconvert(elementd, app::num_processes, app::ffmpeg_prog, app::mkvextract_prog, app::mkvmerge_prog));
	check_source = g_timeout_add(100, dialogconversion::check_do_communication, (gpointer) this);
	
	mc->start();

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
    
    inst->num_completed += boost::numeric_cast<int>(inst->mc->progressd.size());
	
	if (inst->num_completed == inst->num_total) {
	    inst->done();
	    inst->check_source = 0;
		return FALSE;
	}
	else {
	    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(inst->progressbar), float(inst->num_completed) / float(inst->num_total));
	}
	
	inst->mc->progressd.clear();
	
	return TRUE;
}

int dialogconversion::check_mc_has_paused(gpointer self) {
	dialogconversion *inst = (dialogconversion*) self;

	if (inst->mc->ctl.completed_stop()) {
	    gtk_widget_destroy(GTK_WIDGET(inst->dialog));
	    return FALSE;
	}
	
	return TRUE;
}

void dialogconversion::done() {
	gtk_label_set_text(GTK_LABEL(label), "Fine");
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), 1.0f);
	
	gtk_widget_set_sensitive(button_pause, FALSE);
	
	gtk_stack_set_visible_child_name(GTK_STACK(stack), "page_done");
	
	gtk_button_set_label(GTK_BUTTON(button_stop), "_Chiudi");
	gtk_button_set_image(GTK_BUTTON(button_stop), image_button_done);
	
	state = state_done;
	
	//append_to_textview("\nFine", true, false);
}

void dialogconversion::cb_button_stop(GtkButton *, gpointer self) {
	dialogconversion *dialog = (dialogconversion*) self;
	
	if (dialog->state == state_done) {
		gtk_widget_destroy(GTK_WIDGET(dialog->dialog));
	}
	else if (dialog->state == state_converting) {
	    dialog->state = state_pausing;
	    gtk_button_set_label(GTK_BUTTON(dialog->button_stop), "Attendi...");
	    gtk_widget_set_sensitive(dialog->button_stop, FALSE);
	    
	    dialog->mc->ctl.start_stop();
	    
	    g_timeout_add(500, dialogconversion::check_mc_has_paused, self);
	}
}

void dialogconversion::cb_button_pause(GtkButton *, gpointer self) {
}

dialogconversion::~dialogconversion() {
    if (check_source > 0) {
        g_source_remove(check_source);
        check_source = 0;
    }
	g_object_unref(builder);
}

void dialogconversion::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogconversion *inst = (dialogconversion*) self;
	assert(dialog == inst->dialog);
	delete inst;
}

