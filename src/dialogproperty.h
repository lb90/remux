#ifndef REMUX_DIALOGPROPERTY_H
#define REMUX_DIALOGPROPERTY_H

#include <gtk/gtk.h>
#include "gtkbasicmodel.h"
#include "elements.h"

class dialogproperty_t {
public:
	explicit dialogproperty_t(GtkWindow *window);
	~dialogproperty_t();

	void init();
	void show();
	
	int cancel();
	int setcurrentelement(int n);

private:
	GtkBuilder *builder;
	GtkDialog  *dialog;
	
	GtkWidget  *stack;
	
	GtkWidget  *label_name;
	GtkWidget  *check_convert_ac3ita_aac;
	GtkWidget  *check_remove_ac3;
	GtkWidget  *check_remove_dolby;
	GtkWidget  *treeview;
	GtkWidget  *entry_outname;
	
	GtkWidget  *textview_error;
	
	GtkWidget  *button_enqueue;
	GtkWidget  *button_skip;
	GtkWidget  *button_op;
	GtkWidget  *button_moveup;
	GtkWidget  *button_movedown;
	GtkWidget  *button_copy;
	GtkWidget  *button_delete;
	GtkWidget  *button_resetall;
	
	GtkWidget  *treeview_menu;
	
	int      curn;
	media_t *curelem;
	
	void gonext();

	void setnewtreeviewmodel(int numrows);
	
	int getselection();
	void select(int n);
	void selectnone();
	void update_view();
	void change_num_rows();
	void row_changed(int n);
	
	int get_n_from_pathstr(const char *pathstr);
	int get_n_from_path(GtkTreePath *path);
	int get_n_from_iter(GtkTreeIter *iter);

	static
	void cb_edited_name(GtkCellRendererText *ren,
                        gchar               *pathstr,
                        gchar               *new_text,
                        gpointer             self);

	static
	void cb_edited_codec(GtkCellRendererText *ren,
                         gchar               *pathstr,
                         gchar               *new_text,
                         gpointer             self);

	static
	void cb_edited_language(GtkCellRendererText *ren,
                            gchar               *pathstr,
                            gchar               *new_text,
                            gpointer             self);

	static
	void cb_editing_started_codec(GtkCellRenderer *ren,
                                  GtkCellEditable *celleditable,
                                  const gchar     *pathstr,
                                  gpointer         self);

	static
	void cb_editing_started_language(GtkCellRenderer *ren,
                                     GtkCellEditable *celleditable,
                                     const gchar     *pathstr,
                                     gpointer         self);

	static
	void cb_editing_changed_language(GtkCellRendererCombo *combo,
                                     gchar                *pathstr,
                                     GtkTreeIter          *seliter,
                                     gpointer              self);

	static
	void cb_toggled_isdefault(GtkCellRendererToggle *ren, gchar *pathstr, gpointer self);
	
	static
	void cb_toggled_isforced(GtkCellRendererToggle *ren, gchar *pathstr, gpointer self);
	
	static
	void self_deleter(GtkDialog *dialog, gpointer self);
	
	static
	void cb_enqueue(GtkButton *, gpointer self);
	
	static
	void cb_skip(GtkButton *, gpointer self);
	
	static
	void cb_op(GtkButton *, gpointer self);
	
	static
	void cb_copy(GtkButton *, gpointer self);
	
	static
	void cb_delete(GtkButton *, gpointer self);
	
	static
	void cb_resetall(GtkButton *, gpointer self);
	
	static
	void cb_moveup(GtkButton *, gpointer self);
	
	static
	void cb_movedown(GtkButton *, gpointer self);
	
	static
	gboolean cb_treeview_buttonpress(GtkWidget *treeview, GdkEvent *event, gpointer self);
	
	static
	gboolean cb_treeview_popupmenu(GtkWidget *treeview, gpointer self);

	void util_treeview_do_popup_menu(GdkEvent* event);
	
	static
	void
	cell_data_number(GtkTreeViewColumn *,
	                 GtkCellRenderer *ren,
	                 GtkTreeModel *,
	                 GtkTreeIter *iter,
	                 gpointer);
	static
	void
	cell_data_name(GtkTreeViewColumn *,
	               GtkCellRenderer *ren,
	               GtkTreeModel *,
	               GtkTreeIter *iter,
	               gpointer);
	static
	void
	cell_data_type(GtkTreeViewColumn *,
	               GtkCellRenderer *ren,
	               GtkTreeModel *,
	               GtkTreeIter *iter,
	               gpointer);
	static
	void
	cell_data_codec(GtkTreeViewColumn *,
	                GtkCellRenderer *ren,
	                GtkTreeModel *,
	                GtkTreeIter *iter,
	                gpointer);
	static
	void
	cell_data_language(GtkTreeViewColumn *,
	                   GtkCellRenderer *ren,
	                   GtkTreeModel *,
	                   GtkTreeIter *iter,
	                   gpointer);
	static
	void
	cell_data_isdefault(GtkTreeViewColumn *,
	                    GtkCellRenderer *ren,
	                    GtkTreeModel *,
	                    GtkTreeIter *iter,
	                    gpointer);
	static
	void
	cell_data_isforced(GtkTreeViewColumn *,
	                   GtkCellRenderer *ren,
	                   GtkTreeModel *,
	                   GtkTreeIter *iter,
	                   gpointer);
};

#endif

