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
	
	entry_mkvtoolnix = GTK_WIDGET(gtk_builder_get_object(builder, "entry_mkvtoolnix"));
	entry_ac3to      = GTK_WIDGET(gtk_builder_get_object(builder, "entry_ac3to"));
	entry_ffmpeg     = GTK_WIDGET(gtk_builder_get_object(builder, "entry_ffmpeg"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
	g_signal_connect(dialog, "response", G_CALLBACK(response), (gpointer) this);
	
	gtk_entry_set_text(GTK_ENTRY(entry_mkvtoolnix), settings::pt.get("mkvtoolnix", "").c_str());
	gtk_entry_set_text(GTK_ENTRY(entry_mkvtoolnix), settings::pt.get("ac3to", "").c_str());
	gtk_entry_set_text(GTK_ENTRY(entry_mkvtoolnix), settings::pt.get("ffmpeg", "").c_str());
}

void dialogsettings_t::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}

void dialogsettings_t::response(GtkDialog *dialog, gint resp_id, gpointer self) {
	dialogsettings_t *inst = (dialogsettings_t*) self;
	assert(dialog == inst->dialog);
	
	if (resp_id == GTK_RESPONSE_OK) {
		for (const auto& pair : applymap) {
			settings::pt[pair.first] = pair.second;
		}
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

