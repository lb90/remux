#include <cstdlib>
#include <cassert>
#include <climits>
#include <string>
#include <gtk/gtk.h>
#include "app.h"
#include "settings.h"
#include "dialogsettings.h"

dialogsettings_t::dialogsettings_t(GtkWindow *window)

{
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/settings.ui");
	dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));
	
	chooser_mkvtoolnix =  GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "chooser_mkvtoolnix"));
	chooser_ffmpeg     =  GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "chooser_ffmpeg"));
	chooser_log        =  GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "chooser_log"));
	listbox            =      GTK_LIST_BOX(gtk_builder_get_object(builder, "listbox"));
	check_show_window  = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "check_show_window"));
	spinbutton_num_processes = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_num_processes"));
	button_tag_add     =        GTK_BUTTON(gtk_builder_get_object(builder, "button_tag_add"));
	button_tag_remove  =        GTK_BUTTON(gtk_builder_get_object(builder, "button_tag_remove"));
	popover_tag        =       GTK_POPOVER(gtk_builder_get_object(builder, "popover_tag"));
	popover_tag_button =        GTK_BUTTON(gtk_builder_get_object(builder, "popover_tag_button"));
	popover_tag_entry  =         GTK_ENTRY(gtk_builder_get_object(builder, "popover_tag_entry"));
	button_reset_mkvtoolnix =   GTK_BUTTON(gtk_builder_get_object(builder, "button_reset_mkvtoolnix"));
	button_reset_ffmpeg =       GTK_BUTTON(gtk_builder_get_object(builder, "button_reset_ffmpeg"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
	g_signal_connect(dialog, "response", G_CALLBACK(response), (gpointer) this);
	
	g_signal_connect(button_reset_mkvtoolnix, "clicked", G_CALLBACK(reset_mkvtoolnix), (gpointer) this);
	g_signal_connect(button_reset_ffmpeg, "clicked", G_CALLBACK(reset_ffmpeg), (gpointer) this);
	g_signal_connect(button_tag_add, "clicked", G_CALLBACK(show_popover), (gpointer) this);
	g_signal_connect(button_tag_remove, "clicked", G_CALLBACK(remove_tag), (gpointer) this);
	g_signal_connect(popover_tag_button, "clicked", G_CALLBACK(add_tag), (gpointer) this);
	
	g_signal_connect(chooser_mkvtoolnix, "file-set", G_CALLBACK(set_mkvtoolnix), (gpointer) this);
	g_signal_connect(chooser_ffmpeg, "file-set", G_CALLBACK(set_ffmpeg), (gpointer) this);
	
	if (!app::mkvtoolnix_dir.empty())
		gtk_file_chooser_set_filename(chooser_mkvtoolnix, app::mkvtoolnix_dir.c_str());
	else
		gtk_file_chooser_set_filename(chooser_mkvtoolnix, "UNKNOWN");
	if (!app::ffmpeg_dir.empty())
		gtk_file_chooser_set_filename(chooser_ffmpeg, app::ffmpeg_dir.c_str());
	else
		gtk_file_chooser_set_filename(chooser_ffmpeg, "UNKNWON");
	gtk_file_chooser_set_filename(chooser_log, app::log_dir.c_str());
	
	numtags = 0;
	for (const std::string& tag : app::subtitletags) {
		append_listbox(tag);
	}
	
	gtk_toggle_button_set_active(check_show_window, app::showwindow);
	gtk_adjustment_set_value(gtk_spin_button_get_adjustment(spinbutton_num_processes), app::num_processes);
	
	resetted_mkv = app::mkvtoolnix_dir.empty() ? true : false;
	resetted_ff = app::ffmpeg_dir.empty() ? true : false;
}

void dialogsettings_t::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}

void dialogsettings_t::append_listbox(const std::string& tag) {
	GtkWidget *label;
		
	label = gtk_label_new(tag.c_str());
	gtk_widget_set_visible(label, TRUE);
	gtk_container_add(GTK_CONTAINER(listbox), label);
	numtags++;
}

void dialogsettings_t::set_mkvtoolnix(GtkFileChooserButton*, gpointer self) {
	dialogsettings_t *inst;
	
	inst = (dialogsettings_t*) self;
	inst->resetted_mkv = false;
}

void dialogsettings_t::set_ffmpeg(GtkFileChooserButton*, gpointer self) {
	dialogsettings_t *inst;
	
	inst = (dialogsettings_t*) self;
	inst->resetted_ff = false;
}

void dialogsettings_t::reset_mkvtoolnix(GtkButton *, gpointer self) {
	dialogsettings_t *inst;
	
	inst = (dialogsettings_t*) self;

	app::mkvtoolnix_dir = "";
	gtk_file_chooser_set_filename(inst->chooser_mkvtoolnix, "UNKNOWN");
	inst->resetted_mkv = true;
}

void dialogsettings_t::reset_ffmpeg(GtkButton *, gpointer self) {
	dialogsettings_t *inst;
	
	inst = (dialogsettings_t*) self;
	
	app::ffmpeg_dir = "";
	gtk_file_chooser_set_filename(inst->chooser_ffmpeg, "UNKNOWN");
	inst->resetted_ff = true;
}

void dialogsettings_t::show_popover(GtkButton *, gpointer self) {
	dialogsettings_t *inst;
	
	inst = (dialogsettings_t*) self;
	
	gtk_popover_popup(inst->popover_tag);
}

void dialogsettings_t::add_tag(GtkButton *, gpointer self) {
	dialogsettings_t *inst;
	
	inst = (dialogsettings_t*) self;
	
	std::string tag;
	tag = gtk_entry_get_text(inst->popover_tag_entry);
	if (!tag.empty())
		inst->append_listbox(tag);
	
	gtk_popover_popdown(inst->popover_tag);
}

void dialogsettings_t::remove_tag(GtkButton *, gpointer self) {
	dialogsettings_t *inst;
	
	inst = (dialogsettings_t*) self;
	
	GtkListBoxRow *listboxrow;
	listboxrow = gtk_list_box_get_selected_row(inst->listbox);
	if (!listboxrow) return;
	gtk_container_remove(GTK_CONTAINER(inst->listbox), GTK_WIDGET(listboxrow));
	(inst->numtags)--;
}

void dialogsettings_t::response(GtkDialog *dialog, gint resp_id, gpointer self) {
	dialogsettings_t *inst = (dialogsettings_t*) self;
	assert(dialog == inst->dialog);
	
	if (resp_id == GTK_RESPONSE_OK) {
		char *filename;
		
		if (!(inst->resetted_mkv)) {
			filename = gtk_file_chooser_get_filename(inst->chooser_mkvtoolnix);
			app::mkvtoolnix_dir = filename;
			g_free(filename);
		}
		else {
			app::mkvtoolnix_dir.clear();
		}
		if (!(inst->resetted_ff)) {
			filename = gtk_file_chooser_get_filename(inst->chooser_ffmpeg);
			app::ffmpeg_dir = filename;
			g_free(filename);
		}
		else {
			app::ffmpeg_dir.clear();
		}

        filename = gtk_file_chooser_get_filename(inst->chooser_log);
        app::log_dir = filename;
        g_free(filename);
		
		/*TODO*/
		/*iterate over listbox*/
		app::subtitletags.clear();
		for (int i = 0; i < 200; i++) {
			GtkListBoxRow *listboxrow;
			listboxrow = gtk_list_box_get_row_at_index(inst->listbox, i);
			if (listboxrow == NULL)
				break;
			
			GtkLabel *label;
			label = GTK_LABEL(gtk_bin_get_child(GTK_BIN(listboxrow)));
			
			app::subtitletags.emplace_back(gtk_label_get_text(label));
		}

		app::showwindow = gtk_toggle_button_get_active(inst->check_show_window);
		app::num_processes = gtk_adjustment_get_value(gtk_spin_button_get_adjustment(inst->spinbutton_num_processes));
		if (app::num_processes < 0 || app::num_processes > 100)
		    app::num_processes = 1;

		settings::write();
	}
	
	gtk_widget_destroy(GTK_WIDGET(inst->dialog));
}

void dialogsettings_t::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogsettings_t *inst = (dialogsettings_t*) self;
	assert(dialog == inst->dialog);
	delete inst;
}

dialogsettings_t::~dialogsettings_t() {
	g_object_unref(builder);
}

