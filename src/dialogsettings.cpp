#include <cstdlib>
#include <cassert>
#include <climits>
#include <string>
#include <gtk/gtk.h>
#include "settings.h"
#include "dialogsettings.h"

dialogsettings_t::dialogsettings_t(GtkWindow *window)
{
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/settings.ui");
	
	dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));
	
	chooser_mkvtoolnix = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "chooser_mkvtoolnix"));
	chooser_ffmpeg     = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "chooser_ffmpeg"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
	g_signal_connect(dialog, "response", G_CALLBACK(response), (gpointer) this);
	
	std::string filename;
	filename = settings::pt.get("dir.mkvtoolnix", "UNEXISTING");
	gtk_file_chooser_set_filename(chooser_mkvtoolnix, filename.c_str());
	filename = settings::pt.get("dir.ffmpeg", "UNEXISTING");
	gtk_file_chooser_set_filename(chooser_ffmpeg, filename.c_str());
}

void dialogsettings_t::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}

void dialogsettings_t::response(GtkDialog *dialog, gint resp_id, gpointer self) {
	dialogsettings_t *inst = (dialogsettings_t*) self;
	assert(dialog == inst->dialog);
	
	if (resp_id == GTK_RESPONSE_OK) {
		gchar *filename;
		
		filename = gtk_file_chooser_get_filename(inst->chooser_mkvtoolnix);
		settings::pt.put("dir.mkvtoolnix", filename);
		g_free(filename);
		filename = gtk_file_chooser_get_filename(inst->chooser_ffmpeg);
		settings::pt.put("dir.ffmpeg", filename);
		g_free(filename);

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

