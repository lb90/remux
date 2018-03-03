#ifndef REMUX_DIALOGSETTINGS_H
#define REMUX_DIALOGSETTINGS_H

#include <gtk/gtk.h>


class dialogsettings_t {
public:
	explicit dialogsettings_t(GtkWindow *window);
	~dialogsettings_t();

	void show();
	
private:
	GtkBuilder *builder;
	GtkDialog  *dialog;
	
	GtkFileChooser     *chooser_mkvtoolnix;
	GtkFileChooser     *chooser_ffmpeg;
	GtkFileChooser     *chooser_log;
	GtkButton          *button_reset_mkvtoolnix;
	GtkButton          *button_reset_ffmpeg;

	GtkListBox         *listbox;
	GtkButton          *button_tag_add;
	GtkButton          *button_tag_remove;
	
	GtkPopover         *popover_tag;
	GtkButton          *popover_tag_button;
	GtkEntry           *popover_tag_entry;
	
	GtkToggleButton    *check_show_window;
	GtkSpinButton      *spinbutton_num_processes;
	
	bool resetted_mkv;
	bool resetted_ff;
	
	int numtags;
	
	void append_listbox(const std::string& tag);
	
	static
	void set_mkvtoolnix(GtkFileChooserButton*, gpointer self);
	
	static
	void set_ffmpeg(GtkFileChooserButton*, gpointer self);
	
	static
	void reset_mkvtoolnix(GtkButton *, gpointer self);
	
	static
	void reset_ffmpeg(GtkButton *, gpointer self);
	
	static
	void show_popover(GtkButton *, gpointer self);
	
	static
	void add_tag(GtkButton *, gpointer self);
	
	static
	void remove_tag(GtkButton *, gpointer self);
	
	static
	void response(GtkDialog *dialog, gint resp_id, gpointer self);
	
	static
	void self_deleter(GtkDialog *dialog, gpointer self);
};

#endif

