#include <cstdlib>
#include <cassert>
#include <utility>
#include <algorithm>
#include "app.h"
#include "dialogproperty.h"
#include "util.h"
#include "model.h"
#include "elements.h"
#include "treeview.h"
#include "op.h"

dialogproperty_t::dialogproperty_t(GtkWindow *window)
{
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/property.ui");
	
	dialog               = GTK_DIALOG(gtk_builder_get_object(builder, "dialog_property"));
	stack                = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
	label_name           = GTK_WIDGET(gtk_builder_get_object(builder, "label_name"));
	label_position       = GTK_WIDGET(gtk_builder_get_object(builder, "label_position"));
	check_convert_ac3ita_aac = GTK_WIDGET(gtk_builder_get_object(builder, "check_convert_ac3ita_aac"));
	check_remove_ac3     = GTK_WIDGET(gtk_builder_get_object(builder, "check_remove_ac3"));
	check_remove_dolby   = GTK_WIDGET(gtk_builder_get_object(builder, "check_remove_dolby"));
	treeview             = GTK_WIDGET(gtk_builder_get_object(builder, "treeview"));
	entry_outname        = GTK_WIDGET(gtk_builder_get_object(builder, "entry_outname"));
	textview_error       = GTK_WIDGET(gtk_builder_get_object(builder, "textview_error"));
	button_enqueue       = GTK_WIDGET(gtk_builder_get_object(builder, "button_enqueue"));
	button_skip          = GTK_WIDGET(gtk_builder_get_object(builder, "button_skip"));
	button_op            = GTK_WIDGET(gtk_builder_get_object(builder, "button_op"));
	button_moveup        = GTK_WIDGET(gtk_builder_get_object(builder, "button_moveup"));
	button_movedown      = GTK_WIDGET(gtk_builder_get_object(builder, "button_movedown"));
	button_copy          = GTK_WIDGET(gtk_builder_get_object(builder, "button_copy"));
	button_delete        = GTK_WIDGET(gtk_builder_get_object(builder, "button_delete"));
	button_resetall      = GTK_WIDGET(gtk_builder_get_object(builder, "button_resetall"));
	treeview_menu        = GTK_WIDGET(gtk_builder_get_object(builder, "menu_treeview"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
	g_signal_connect(button_enqueue, "clicked", G_CALLBACK(cb_enqueue), (gpointer) this);
	g_signal_connect(button_skip, "clicked", G_CALLBACK(cb_skip), (gpointer) this);
	g_signal_connect(button_op, "clicked", G_CALLBACK(cb_op), (gpointer) this);
	g_signal_connect(button_moveup, "clicked", G_CALLBACK(cb_moveup), (gpointer) this);
	g_signal_connect(button_movedown, "clicked", G_CALLBACK(cb_movedown), (gpointer) this);
	g_signal_connect(button_copy, "clicked", G_CALLBACK(cb_copy), (gpointer) this);
	g_signal_connect(button_delete, "clicked", G_CALLBACK(cb_delete), (gpointer) this);
	g_signal_connect(button_resetall, "clicked", G_CALLBACK(cb_resetall), (gpointer) this);
	
	g_signal_connect(treeview, "button-press-event", G_CALLBACK(cb_treeview_buttonpress), (gpointer) this);
    g_signal_connect(treeview, "popup-menu", G_CALLBACK(cb_treeview_popupmenu), (gpointer) this);
	
	GtkTreeViewColumn *col;
	GtkCellRenderer   *ren;
	std::vector<std::pair<std::string, GtkTreeCellDataFunc>> colv = {
		std::make_pair("number",    cell_data_number),
		std::make_pair("name",      cell_data_name),
		std::make_pair("type",      cell_data_type),
		std::make_pair("codec",     cell_data_codec),
		std::make_pair("language",  cell_data_language),
		std::make_pair("isdefault", cell_data_isdefault),
		std::make_pair("isforced",  cell_data_isforced)
	};
	std::string colstring = "col_";
	std::string renstring = "ren_";
	for (const auto& c : colv) {
		col = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object( builder, (colstring + c.first).c_str() ));
		ren = GTK_CELL_RENDERER(gtk_builder_get_object( builder, (renstring + c.first).c_str() ));
		gtk_tree_view_column_set_cell_data_func(col, ren, c.second, (gpointer) this, NULL);
	}
	
	ren = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "ren_name"));
	g_signal_connect(ren, "edited", G_CALLBACK(cb_edited_name), (gpointer) this);
	ren = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "ren_codec"));
	g_object_set(ren, "text-column", 0, NULL);
	g_signal_connect(ren, "edited", G_CALLBACK(cb_edited_codec), (gpointer) this);
	g_signal_connect(ren, "editing-started", G_CALLBACK(cb_editing_started_codec), (gpointer) this);
	ren = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "ren_language"));
	g_object_set(ren, "text-column", 0, NULL);
	g_signal_connect(ren, "edited", G_CALLBACK(cb_edited_language), (gpointer) this);
	g_signal_connect(ren, "editing-started", G_CALLBACK(cb_editing_started_language), (gpointer) this);
	//g_signal_connect(ren, "changed", G_CALLBACK(cb_editing_changed_language), (gpointer) this);
	ren = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "ren_isdefault"));
	g_signal_connect(ren, "toggled", G_CALLBACK(cb_toggled_isdefault), (gpointer) this);
	ren = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "ren_isforced"));
	g_signal_connect(ren, "toggled", G_CALLBACK(cb_toggled_isforced), (gpointer) this);
	
	curn = -1;
	curelem = nullptr;
	
	menu_i = 0;
	menu_j = 0;
	
	gtk_application_add_window(app::gtkapp, GTK_WINDOW(dialog));
	app::dialogproperty = this;
	
	app::suspend_conversion_timer();
}

void dialogproperty_t::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}

int dialogproperty_t::setcurrentelement(gint n) {
	assert(n >= 0);
	assert(size_t(n) < elementv.size());
	
	if (curn == n)
		return 0;

	cancel();
	curelem = &(elementv[n]);
	curn = n;
	
	treeview_select_n(curn);
	
	if (!curelem->isinit)
		op::media_scan(n);
	
	gtk_label_set_text(GTK_LABEL(label_name), curelem->name.c_str());
	std::string position;
	position += std::to_string(curn + 1);
	position += " / ";
	position += std::to_string(elementv.size());
	gtk_label_set_text(GTK_LABEL(label_position), position.c_str());

	if (curelem->err.scan) {
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "page_error");

		gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_error)),
		                         curelem->err.scan_description.c_str(),
		                         -1);
		
		gtk_widget_set_sensitive(button_enqueue, FALSE);
	}
	else {
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "page_main");

		assert(curelem->destitems.size() < INT_MAX);
		setnewtreeviewmodel(int(curelem->destitems.size()));

		gtk_entry_set_text(GTK_ENTRY(entry_outname), curelem->outname.c_str());
		
		gtk_widget_set_sensitive(button_enqueue, TRUE);
	}
	
	return 0;
}

int dialogproperty_t::cancel() {
	gtk_label_set_text(GTK_LABEL(label_name), "");
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), NULL);
	gtk_entry_set_text(GTK_ENTRY(entry_outname), "");

	curn = -1;
	curelem = nullptr;

	return 0;
}

void dialogproperty_t::setnewtreeviewmodel(int numrows) {
	BasicListModel *basic_model;

	basic_model = basic_list_model_new(curelem->destitems.size());

	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(basic_model));
	g_object_unref(basic_model);
}

int dialogproperty_t::getselection() {
	GtkTreeSelection   *treesel;
	GtkTreeIter         iter = {};
	
	treesel = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (gtk_tree_selection_get_selected(treesel, NULL, &iter)) {
		int n;
		
		n = GPOINTER_TO_INT(iter.user_data);
		assert(n >= 0 && size_t(n) < curelem->destitems.size());
		return n;
	}
	else return -1;
}

void dialogproperty_t::select(int n) {
	BasicListModel     *basic_model;
	GtkTreeSelection   *treesel;
	GtkTreeIter         iter;
	
	basic_model = BASIC_LIST_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
	treesel = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	iter.stamp = basic_model->stamp;
	iter.user_data = GINT_TO_POINTER(n);
	gtk_tree_selection_select_iter(treesel, &iter);
}

void dialogproperty_t::selectnone() {
	GtkTreeSelection   *treesel;
	treesel = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	gtk_tree_selection_unselect_all(treesel);
}

void dialogproperty_t::scrollto(int n, bool totop) {
	GtkTreePath *path;
	double row_align;

	row_align = totop ? 0.0f : 1.0f;

	path = get_path_from_n(n);
	
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(treeview),
	                             path,
	                             NULL,
	                             TRUE,
	                             row_align,
	                             0.0f);
	gtk_tree_path_free(path);
}

void dialogproperty_t::update_view() {
	BasicListModel *basic_model;
	basic_model = BASIC_LIST_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
	basic_list_model_emit_changed_all(basic_model);
}

void dialogproperty_t::change_num_rows() {
	BasicListModel *basic_model;
	basic_model = BASIC_LIST_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
	
	assert(curelem->destitems.size() <= INT_MAX);
	int newnumrows = int(curelem->destitems.size());
	
	basic_list_model_set_new_num_rows(basic_model, newnumrows);
}

void dialogproperty_t::row_changed(int n) {
	BasicListModel *basic_model;
	basic_model = BASIC_LIST_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
	
	assert(n >= 0);
	assert(size_t(n) < curelem->destitems.size());
	
	basic_list_model_emit_row_changed(basic_model, n);
}

GtkTreePath *dialogproperty_t::get_path_from_n(int n) {
	GtkTreePath *path;
	path = gtk_tree_path_new();
	gtk_tree_path_append_index(path, n);
	return path;
}

int dialogproperty_t::get_n_from_pathstr(const char *pathstr) {
	GtkTreePath *path;
	int n;
	
	assert(pathstr);

	path = gtk_tree_path_new_from_string(pathstr);
	n = get_n_from_path(path);
	gtk_tree_path_free(path);
	
	return n;
}

int dialogproperty_t::get_n_from_path(GtkTreePath *path) {
	int n, *indices, depth;
	
	assert(path); /*TODO*/
	
	indices = gtk_tree_path_get_indices_with_depth(path, &depth);
	assert(depth == 1);
	assert(indices);
	n = indices[0];
	assert(n >= 0);
	
	return n;
}

int dialogproperty_t::get_n_from_iter(GtkTreeIter *iter) {
	int n;
	n = GPOINTER_TO_INT(iter->user_data);
	assert(n >= 0);
	
	return n;
}

void dialogproperty_t::gonext() {
	int n = curn;

	cancel();

	assert(n >= 0);
	if (size_t(n + 1) < elementv.size())
		setcurrentelement(n + 1);
	else
		gtk_widget_destroy(GTK_WIDGET(dialog));
}

void dialogproperty_t::cb_enqueue(GtkButton *, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	
	op::enqueue(inst->curn);
	
	inst->gonext();
}

void dialogproperty_t::cb_skip(GtkButton *, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	
	inst->gonext();
}

void dialogproperty_t::cb_op(GtkButton *, gpointer self) {
	bool want_convert_ac3ita_aac;
	bool want_remove_ac3;
	bool want_remove_dolby;	

	dialogproperty_t *inst = (dialogproperty_t*) self;
	assert(inst && inst->curelem);
	
	want_convert_ac3ita_aac = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(inst->check_convert_ac3ita_aac));
	want_remove_ac3 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(inst->check_remove_ac3));
	want_remove_dolby = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(inst->check_remove_dolby));
	
	media_t& elem = *(inst->curelem);
	if (elem.destitems.empty()) return;

/* separate video, audio, subtitle and other tracks */
	std::vector<destitem_t> videoitems;
	std::vector<destitem_t> audioitems;
	std::vector<destitem_t> subtitleitems;
	std::vector<destitem_t> otheritems;
	
	for (const destitem_t& item : elem.destitems) {
		if (item.type == itemtype_video)
			videoitems.emplace_back(item);
		else if (item.type == itemtype_audio)
			audioitems.emplace_back(item);
		else if (item.type == itemtype_subtitle)
			subtitleitems.emplace_back(item);
		else
			otheritems.emplace_back(item);
	}

/* set video tracks as ita */
	for (destitem_t& item : videoitems) {
		item.lang = "ita";
	}
	
/* from audioitems, separate ac3, dolby and every other */
	std::vector<destitem_t> audioitems_ac3;
	std::vector<destitem_t> audioitems_dolby;
	std::vector<destitem_t> audioitems_not_ac3_nor_dolby;
	for (const destitem_t& item : audioitems) {
		if ( (item.codecid == codecid_ac3 || item.codecid == codecid_eac3) )
			audioitems_ac3.emplace_back(item);
		else if ( (item.codecid == codecid_dts || item.codecid == codecid_truehd) )
			audioitems_dolby.emplace_back(item);
		else
			audioitems_not_ac3_nor_dolby.emplace_back(item);
	}

/* from audio ac3 tracks, separate ita and every other */
	std::vector<destitem_t> audioac3items_ita;
	std::vector<destitem_t> audioac3items_not_ita;
	for (const destitem_t& item : audioitems_ac3) {
		if ( item.lang == "ita" )
			audioac3items_ita.emplace_back(item);
		else
			audioac3items_not_ita.emplace_back(item);
	}

/* if you want to convert ac3 ita to aac, create copy tracks with aac codec */
	std::vector<destitem_t> aacconverted;
	if (want_convert_ac3ita_aac) {
		for (destitem_t& item : audioac3items_ita) {
			aacconverted.emplace_back(item);
		}
		for (destitem_t& item : aacconverted) { /* set their properties */
			item.codecid = codecid_aac;
			item.isdefault = true;
			item.isforced = true;
		}
	}

/* from audio dolby tracks, separate ita and every other */
	std::vector<destitem_t> audiodolbyitems_ita;
	std::vector<destitem_t> audiodolbyitems_not_ita;
	for (const destitem_t& item : audioitems_dolby) {
		if ( item.lang == "ita" )
			audiodolbyitems_ita.emplace_back(item);
		else
			audiodolbyitems_not_ita.emplace_back(item);
	}

/* merge back audio items as 1) aac converted 2) ac3 ita 3) ac3 other langs
   4) dolby ita 5) dolby other langs 6) others */
	audioitems.clear();
	for (const destitem_t& item : aacconverted)
		audioitems.emplace_back(item);
	if (!want_remove_ac3) {
		for (const destitem_t& item : audioac3items_ita)
			audioitems.emplace_back(item);
		for (const destitem_t& item : audioac3items_not_ita)
			audioitems.emplace_back(item);
	}
	if (!want_remove_dolby) {
		for (const destitem_t& item : audiodolbyitems_ita)
			audioitems.emplace_back(item);
		for (const destitem_t& item : audiodolbyitems_not_ita)
			audioitems.emplace_back(item);
	}
	for (const destitem_t& item : audioitems_not_ac3_nor_dolby)
		audioitems.emplace_back(item);
	
	elem.destitems.clear();
	
	for (destitem_t& item : videoitems)
		elem.destitems.emplace_back(item);

	for (destitem_t& item : audioitems)
		elem.destitems.emplace_back(item);
	
	for (destitem_t& item : subtitleitems)
		elem.destitems.emplace_back(item);

/* in case ac3 tracks or dolby tracks were saved, set them NOT default and NOT forced */
	for (destitem_t& item : elem.destitems) {
		if (item.type == itemtype_audio) {
			if ( item.codecid == codecid_ac3 || item.codecid == codecid_eac3 ) {
				item.isdefault = false;
				item.isforced = false;
			}
		}
	}
	
	for (destitem_t& item : elem.destitems) {
		if (item.type == itemtype_audio) {
			if ( item.codecid == codecid_dts || item.codecid == codecid_truehd ) {
				item.isdefault = false;
				item.isforced = false;
			}
		}
	}

/* set target subtitles as default and forced */
	for (destitem_t& item : elem.destitems) {
		if (item.type == itemtype_subtitle) {
			if (item.lang == "ita") {
				for (const std::string& tag : app::subtitletags) {
					if (item.name.find(tag) != std::string::npos) {
						item.isdefault = true;
						item.isforced = true;
						break;
					}
				}
			}
		}
	}

	inst->change_num_rows();
}

void dialogproperty_t::cb_copy(GtkButton *, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	assert(inst && inst->curelem);
	
	media_t& elem = *(inst->curelem);

	int nn = inst->getselection();
	if (nn < 0) return;
	size_t n = nn;
	
	std::vector<destitem_t> firstpart;
	for (size_t i = 0; i <= n; i++)
		firstpart.emplace_back(elem.destitems[i]);
	
	std::vector<destitem_t> secondpart;
	for (size_t i = n; i < inst->curelem->destitems.size(); i++)
		secondpart.emplace_back(elem.destitems[i]);
	
	elem.destitems.clear();
	
	for (const destitem_t& item : firstpart)
		elem.destitems.emplace_back(item);
	for (const destitem_t& item : secondpart)
		elem.destitems.emplace_back(item);
	
	inst->change_num_rows();
	
	inst->select(nn + 1);
	inst->scrollto(nn + 1, false);
}

void dialogproperty_t::cb_delete(GtkButton *, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	assert(inst && inst->curelem);
	
	media_t& elem = *(inst->curelem);
	
	int nn = inst->getselection();
	if (nn < 0) return;
	size_t n = nn;
	
	std::vector<destitem_t> firstpart;
	for (size_t i = 0; i < n; i++)
		firstpart.emplace_back(elem.destitems[i]);
	
	std::vector<destitem_t> secondpart;
	for (size_t i = n + 1; i < inst->curelem->destitems.size(); i++)
		secondpart.emplace_back(elem.destitems[i]);
	
	elem.destitems.clear();
	
	for (const destitem_t& item : firstpart)
		elem.destitems.emplace_back(item);
	for (const destitem_t& item : secondpart)
		elem.destitems.emplace_back(item);
	
	//inst->selectnone();
	
	inst->change_num_rows();
}

void dialogproperty_t::cb_resetall(GtkButton *, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	assert(inst && inst->curelem);
	
	media_t& elem = *(inst->curelem);
	
	elem.destitems.clear();
	
	for (const origitem_t& origitem : elem.origitems) {
		elem.destitems.emplace_back(origitem);
	}
	
	inst->change_num_rows();
}

void dialogproperty_t::cb_moveup(GtkButton *, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
		
	int nn = inst->getselection();
	if (nn < 0) return
	assert(size_t(nn) < inst->curelem->destitems.size());
	size_t n = nn;
	
	if (n == 0) return;
	size_t m = n - 1;
		
	std::vector<destitem_t> newdestitems;
	for (size_t i = 0; i < inst->curelem->destitems.size(); i++) {
		if (i == m)
			newdestitems.emplace_back(inst->curelem->destitems[n]);
		else if (i == n)
			newdestitems.emplace_back(inst->curelem->destitems[m]);
		else
			newdestitems.emplace_back(inst->curelem->destitems[i]);
	}
		
	inst->curelem->destitems.clear();
	
	for (const destitem_t& item : newdestitems)
		inst->curelem->destitems.emplace_back(item);
		
	inst->update_view();
		
	inst->select(m);
	inst->scrollto(m, true);
}

void dialogproperty_t::cb_movedown(GtkButton *, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
		
	int nn = inst->getselection();
	if (nn < 0) return;
	assert(size_t(nn) < inst->curelem->destitems.size());
	size_t n = nn;
	
	if (n + 1 == inst->curelem->destitems.size()) return;
	size_t m = n + 1;
		
	std::vector<destitem_t> newdestitems;
	for (size_t i = 0; i < inst->curelem->destitems.size(); i++) {
		if (i == m)
			newdestitems.emplace_back(inst->curelem->destitems[n]);
		else if (i == n)
			newdestitems.emplace_back(inst->curelem->destitems[m]);
		else
			newdestitems.emplace_back(inst->curelem->destitems[i]);
	}
		
	inst->curelem->destitems.clear();
		
	for (const destitem_t& item : newdestitems)
		inst->curelem->destitems.emplace_back(item);
		
	inst->update_view();
		
	inst->select(m);
	inst->scrollto(m, false);
}


gboolean dialogproperty_t::cb_treeview_buttonpress(GtkWidget *treeview, GdkEvent *event, gpointer self) {
	dialogproperty_t *inst;
	GdkEventButton *eventbutton;
	
	inst = (dialogproperty_t*) self;
	eventbutton = (GdkEventButton*) event;
	if (eventbutton->type == GDK_BUTTON_PRESS && eventbutton->button == 3) {
		
		
		inst->util_treeview_do_popup_menu(event);
		
		return TRUE;
	}
	
	return FALSE;
}

gboolean dialogproperty_t::cb_treeview_popupmenu(GtkWidget *treeview, gpointer self) {
	dialogproperty_t *inst;
	
	inst = (dialogproperty_t*) self;
	inst->util_treeview_do_popup_menu(NULL);
	return TRUE;
}

void dialogproperty_t::util_treeview_do_popup_menu(GdkEvent* event) {
	if (event) {
		gtk_menu_popup_at_pointer(GTK_MENU(treeview_menu), event);
	}
	else {
		gtk_menu_popup_at_widget(GTK_MENU(treeview_menu),
		                         treeview,
		                         GDK_GRAVITY_NORTH_WEST,
		                         GDK_GRAVITY_NORTH_WEST,
		                         NULL);
	}
}

void dialogproperty_t::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	assert(dialog == inst->dialog);

	delete inst;
}

dialogproperty_t::~dialogproperty_t() {
	g_object_unref(builder);
	app::dialogproperty = nullptr;
	app::resume_conversion_timer();
}







void dialogproperty_t::cb_edited_name(GtkCellRendererText *ren,
                                      gchar               *pathstr,
                                      gchar               *new_text,
                                      gpointer             self)
{
	dialogproperty_t *inst = (dialogproperty_t*) self;
	GtkTreePath      *path;
	gint *indices, depth, n;
	
	inst = (dialogproperty_t*) self;
	
	path = gtk_tree_path_new_from_string(pathstr);
	if (path) {
		depth = gtk_tree_path_get_depth(path);
		assert(depth == 1);
		indices = gtk_tree_path_get_indices(path);
		assert(indices);
		n = indices[0];
		assert(n >= 0);
		assert(size_t(n) < inst->curelem->destitems.size());
			
		inst->curelem->destitems[n].name = new_text;

		BasicListModel *basic_list_model;
		basic_list_model = BASIC_LIST_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(inst->treeview)));
		basic_list_model_emit_row_changed(basic_list_model, n);

		gtk_tree_path_free(path);
	}
}

void dialogproperty_t::cb_edited_codec(GtkCellRendererText *ren,
                                       gchar               *pathstr,
                                       gchar               *new_text,
                                       gpointer             self)
{
	dialogproperty_t *inst = (dialogproperty_t*) self;
	GtkTreePath      *path;
	gint *indices, depth, n;
	
	inst = (dialogproperty_t*) self;
	
	path = gtk_tree_path_new_from_string(pathstr);
	if (path) {
		depth = gtk_tree_path_get_depth(path);
		assert(depth == 1);
		indices = gtk_tree_path_get_indices(path);
		assert(indices);
		n = indices[0];
		assert(n >= 0);
		assert(size_t(n) < inst->curelem->destitems.size());
	
		inst->curelem->destitems[n].codecid = codecid_from_name(new_text);
			
		BasicListModel *basic_list_model;
		basic_list_model = BASIC_LIST_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(inst->treeview)));
		basic_list_model_emit_row_changed(basic_list_model, n);

		gtk_tree_path_free(path);
	}
}

void dialogproperty_t::cb_edited_language(GtkCellRendererText *ren,
                                          gchar               *pathstr,
                                          gchar               *new_text,
                                          gpointer             self)
{
	dialogproperty_t *inst = (dialogproperty_t*) self;
	GtkTreePath      *path;
	gint *indices, depth, n;
	
	inst = (dialogproperty_t*) self;
	
	path = gtk_tree_path_new_from_string(pathstr);
	if (path) {
		depth = gtk_tree_path_get_depth(path);
		assert(depth == 1);
		indices = gtk_tree_path_get_indices(path);
		assert(indices);
		n = indices[0];
		assert(n >= 0);
		assert(size_t(n) < inst->curelem->destitems.size());
			
		std::string newlang = new_text;
		if (newlang == "ita" || newlang == "eng")
			inst->curelem->destitems[n].lang = newlang;
				
		BasicListModel *basic_list_model;
		basic_list_model = BASIC_LIST_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(inst->treeview)));
		basic_list_model_emit_row_changed(basic_list_model, n);
			
		gtk_tree_path_free(path);
	}
}

void dialogproperty_t::cb_editing_started_codec(GtkCellRenderer *ren,
                                                GtkCellEditable *celleditable,
                                                const gchar     *pathstr,
                                                gpointer         self)
{
	dialogproperty_t *inst;
	
	inst = (dialogproperty_t*) self;
	
	if (GTK_IS_COMBO_BOX(celleditable)) {
		GtkComboBox *combo;
		int n;
		
		combo = GTK_COMBO_BOX(celleditable);
		
		gtk_combo_box_set_id_column(combo, 0);
		n = inst->get_n_from_pathstr(pathstr);
		gtk_combo_box_set_active_id(combo, codecid_to_name(inst->curelem->destitems[n].codecid).c_str()); /*TODO*/
	}
}

void dialogproperty_t::cb_editing_started_language(GtkCellRenderer *ren,
                                                   GtkCellEditable *celleditable,
                                                   const gchar     *pathstr,
                                                   gpointer         self)
{
	dialogproperty_t *inst;
	
	inst = (dialogproperty_t*) self;
	
	if (GTK_IS_COMBO_BOX(celleditable)) {
		GtkComboBox *combo;
		int n;
		
		combo = GTK_COMBO_BOX(celleditable);
		
		gtk_combo_box_set_id_column(combo, 0);
		n = inst->get_n_from_pathstr(pathstr);
		gtk_combo_box_set_active_id(combo, inst->curelem->destitems[n].lang.c_str());
	}
}

void dialogproperty_t::cb_editing_changed_language(GtkCellRendererCombo *ren,
                                                   gchar                *pathstr,
                                                   GtkTreeIter          *seliter,
                                                   gpointer              self)
{
/*	dialogproperty_t *inst;
	GtkTreeModel *store;
	char         *selstring;
	int n;
	
	inst = (dialogproperty_t*) self;
	
	g_object_get(ren, "model", &store, NULL);
	gtk_tree_model_get(store, seliter, 0, &selstring, -1);
	
	n = inst->get_n_from_pathstr(pathstr);
	
	inst->curelem->destitems[n].lang = selstring;
	inst->row_changed(n);
	
	g_free(selstring);*/
}

void dialogproperty_t::cb_toggled_isdefault(GtkCellRendererToggle *ren, gchar *pathstr, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	GtkTreePath      *path;
	gint *indices, depth, n;
	
	inst = (dialogproperty_t*) self;
	
	path = gtk_tree_path_new_from_string(pathstr);
	if (path) {
		depth = gtk_tree_path_get_depth(path);
		assert(depth == 1);
		indices = gtk_tree_path_get_indices(path);
		assert(indices);
		n = indices[0];
		assert(n >= 0);
		assert(size_t(n) < inst->curelem->destitems.size());
			
		inst->curelem->destitems[n].isdefault
		  = !inst->curelem->destitems[n].isdefault;

		BasicListModel *basic_list_model;
		basic_list_model = BASIC_LIST_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(inst->treeview)));
		basic_list_model_emit_row_changed(basic_list_model, n);
		gtk_tree_path_free(path);
	}
}

void dialogproperty_t::cb_toggled_isforced(GtkCellRendererToggle *ren, gchar *pathstr, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	GtkTreePath      *path;
	gint *indices, depth, n;
	
	inst = (dialogproperty_t*) self;
	
	path = gtk_tree_path_new_from_string(pathstr);
	if (path) {
		depth = gtk_tree_path_get_depth(path);
		assert(depth == 1);
		indices = gtk_tree_path_get_indices(path);
		assert(indices);
		n = indices[0];
		assert(n >= 0);
		assert(size_t(n) < inst->curelem->destitems.size());
			
		inst->curelem->destitems[n].isforced
		  = !inst->curelem->destitems[n].isforced;
			
		BasicListModel *basic_list_model;
		basic_list_model = BASIC_LIST_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(inst->treeview)));
		basic_list_model_emit_row_changed(basic_list_model, n);
		gtk_tree_path_free(path);
	}
}

static void internal_cell_color(GtkCellRenderer *ren, itemtype_t type) {
	static GdkRGBA lightpurple = {};
	static GdkRGBA lightorange = {};
	static GdkRGBA lightyellow = {};
	static bool firstrun = true;
	
	if (firstrun) {
		gdk_rgba_parse(&lightpurple, "rgb(183,192,232)");
		gdk_rgba_parse(&lightorange, "rgb(242,213,170)");
		gdk_rgba_parse(&lightyellow, "rgb(240,235,190)");
		firstrun = false;
	}
	
	g_object_set(ren, "cell-background-set", TRUE, NULL);
	switch (type) {
		case itemtype_video:
			g_object_set(ren, "cell-background-rgba", &lightpurple, NULL);
			break;
		case itemtype_audio:
			g_object_set(ren, "cell-background-rgba", &lightorange, NULL);
			break;
		case itemtype_subtitle:
			g_object_set(ren, "cell-background-rgba", &lightyellow, NULL);
			break;
		case itemtype_button:
			/*TODO*/
			//g_object_set(ren, "cell-background-rgba", &lightpurple, NULL);
			break;
		default:
			g_object_set(ren, "cell-background-set", FALSE, NULL);
	}
}

void dialogproperty_t::cell_data_number(GtkTreeViewColumn *,
	                                    GtkCellRenderer *ren,
	                                    GtkTreeModel *filter_model,
	                                    GtkTreeIter *it,
	                                    gpointer inst)
{
	gint n = GPOINTER_TO_INT(it->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	const destitem_t& item = self->curelem->destitems[n];

	g_object_set(ren, "text", std::to_string(n+1).c_str(), NULL);

	internal_cell_color(ren, item.type);
}

void dialogproperty_t::cell_data_name(GtkTreeViewColumn *,
	                                  GtkCellRenderer *ren,
	                                  GtkTreeModel *filter_model,
	                                  GtkTreeIter *it,
	                                  gpointer inst)
{
	gint n = GPOINTER_TO_INT(it->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	const destitem_t& item = self->curelem->destitems[n];
	
	std::string text = item.name;
	if (item.name != item.orig.name)
		text = "<span foreground=\"#a40000\">" + text + "</span>";
	g_object_set(ren, "markup", text.c_str(), NULL);

	internal_cell_color(ren, item.type);
}

void dialogproperty_t::cell_data_type(GtkTreeViewColumn *,
	                                  GtkCellRenderer *ren,
	                                  GtkTreeModel *filter_model,
	                                  GtkTreeIter *it,
	                                  gpointer inst)
{
	gint n = GPOINTER_TO_INT(it->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	const destitem_t& item = self->curelem->destitems[n];
	const char *text = "";
	switch (item.type) {
		case itemtype_subtitle:
			text = "Sottotitolo";
			break;
		case itemtype_video:
			text = "Video";
			break;
		case itemtype_audio:
			text = "Audio";
			break;
		default:
			break;
	}
	
	g_object_set(ren, "text", text, NULL);
	internal_cell_color(ren, item.type);
}

void dialogproperty_t::cell_data_codec(GtkTreeViewColumn *,
	                                   GtkCellRenderer *ren,
	                                   GtkTreeModel *filter_model,
	                                   GtkTreeIter *it,
	                                   gpointer inst)
{
	gint n = GPOINTER_TO_INT(it->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	const destitem_t& item = self->curelem->destitems[n];

	if (item.type == itemtype_audio)
		g_object_set(ren, "editable", TRUE, NULL);
	else
		g_object_set(ren, "editable", FALSE, NULL);

	std::string text;
	switch (item.codecid) {
		case codecid_ac3:
			text = "AC-3";
			break;
		case codecid_eac3:
			text = "EAC-3";
			break;
		case codecid_dts:
			text = "DTS";
			break;
		case codecid_truehd:
			text = "TrueHD";
			break;
		case codecid_aac:
			text = "AAC";
			break;
		case codecid_mp2:
			text = "MP2";
			break;
		case codecid_mp3:
			text = "MP3";
			break;
		case codecid_vorbis:
			text = "Vorbis";
			break;
		case codecid_flac:
			text = "FLAC";
			break;
		case codecid_pcm:
			text = "PCM";
			break;
		default:
			text = item.codecname;
	}

	if (item.codecid != item.orig.codecid)
		text = "<span foreground=\"#a40000\">" + text + "</span>";
	
	g_object_set(ren, "markup", text.c_str(), NULL);

	internal_cell_color(ren, item.type);
}

void dialogproperty_t::cell_data_language(GtkTreeViewColumn *,
	                                      GtkCellRenderer *ren,
	                                      GtkTreeModel *filter_model,
	                                      GtkTreeIter *it,
	                                      gpointer inst)
{
	gint n = GPOINTER_TO_INT(it->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	const destitem_t& item = self->curelem->destitems[n];

	std::string text;
	text = item.lang;
	if (item.lang != item.orig.lang)
		text = "<span foreground=\"#a40000\">" + text + "</span>";
	
	g_object_set(ren, "markup", text.c_str(), NULL);
	
	internal_cell_color(ren, item.type);
}

void dialogproperty_t::cell_data_isdefault(GtkTreeViewColumn *,
	                                       GtkCellRenderer *ren,
	                                       GtkTreeModel *filter_model,
	                                       GtkTreeIter *it,
	                                       gpointer inst)
{
	gint n = GPOINTER_TO_INT(it->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	const destitem_t& item = self->curelem->destitems[n];
	gboolean bstate = item.isdefault;
	
	g_object_set(ren, "active", bstate, NULL);
	internal_cell_color(ren, item.type);
}

void dialogproperty_t::cell_data_isforced(GtkTreeViewColumn *,
	                                      GtkCellRenderer *ren,
	                                      GtkTreeModel *filter_model,
	                                      GtkTreeIter *it,
	                                      gpointer inst)
{
	gint n = GPOINTER_TO_INT(it->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	const destitem_t& item = self->curelem->destitems[n];
	gboolean bstate = item.isforced;

	g_object_set(ren, "active", bstate, NULL);
	internal_cell_color(ren, item.type);
}

