#include <cstdlib>
#include <cassert>
#include <utility>
#include "dialogproperty.h"
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
	check_convert_ac3ita_aac = GTK_WIDGET(gtk_builder_get_object(builder, "check_convert_ac3ita_aac"));
	check_keep_ac3       = GTK_WIDGET(gtk_builder_get_object(builder, "check_keep_ac3"));
	check_keep_dolby     = GTK_WIDGET(gtk_builder_get_object(builder, "check_keep_dolby"));
	treeview_item        = GTK_WIDGET(gtk_builder_get_object(builder, "treeview_item"));
	entry_outname        = GTK_WIDGET(gtk_builder_get_object(builder, "entry_outname"));
	textview_error       = GTK_WIDGET(gtk_builder_get_object(builder, "textview_error"));
	button_enqueue       = GTK_WIDGET(gtk_builder_get_object(builder, "button_enqueue"));
	button_skip          = GTK_WIDGET(gtk_builder_get_object(builder, "button_skip"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
	g_signal_connect(button_enqueue, "clicked", G_CALLBACK(cb_enqueue), (gpointer) this);
	g_signal_connect(button_skip, "clicked", G_CALLBACK(cb_skip), (gpointer) this);
	
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
	
	ren = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "ren_isdefault"));
	g_signal_connect(ren, "toggled", G_CALLBACK(cb_toggled_isdefault), (gpointer) this);
	ren = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "ren_isforced"));
	g_signal_connect(ren, "toggled", G_CALLBACK(cb_toggled_isforced), (gpointer) this);
	
	g_signal_connect(check_keep_ac3, "toggled", G_CALLBACK(cb_keep_ac3), (gpointer) this);
	g_signal_connect(check_keep_dolby, "toggled", G_CALLBACK(cb_keep_dolby), (gpointer) this);
	
	curn = -1;
	curelem = nullptr;
}

void dialogproperty_t::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}

gboolean dialogproperty_t::filter_visible_func(GtkTreeModel *childmodel, GtkTreeIter *iter, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	int n = GPOINTER_TO_INT(iter->user_data);
	
	if (n < 0 || size_t(n) >= inst->curelem->destitems.size())
		return TRUE;
	
	return inst->curelem->destitems[n].want;
}

void dialogproperty_t::setnewtreeviewmodel(int numrows) {
	BasicListModel *basic_model;
	GtkTreeModelFilter *filter_model;

	basic_model = basic_list_model_new(curelem->destitems.size());
	filter_model = GTK_TREE_MODEL_FILTER(gtk_tree_model_filter_new(GTK_TREE_MODEL(basic_model), NULL));
	g_object_unref(basic_model);
	
	gtk_tree_model_filter_set_visible_func(filter_model, filter_visible_func, this, NULL);

	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_item), GTK_TREE_MODEL(filter_model));
	g_object_unref(filter_model);
}

void dialogproperty_t::cb_toggled_isdefault(GtkCellRendererToggle *ren, gchar *pathstr, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	GtkTreePath *path;
	gint *indices, depth, n;
	
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
		gtk_tree_path_free(path);
		path = NULL;
	}
}

void dialogproperty_t::cb_toggled_isforced(GtkCellRendererToggle *ren, gchar *pathstr, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	GtkTreePath *path;
	gint *indices, depth, n;
	
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
		gtk_tree_path_free(path);
		path = NULL;
	}
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

	if (curelem->err.scan) {
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "page_error");

		gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_error)),
		                         curelem->err.scan_description.c_str(),
		                         -1);
	}
	else {
		gtk_stack_set_visible_child_name(GTK_STACK(stack), "page_main");

		gtk_label_set_text(GTK_LABEL(label_name), curelem->name.c_str());

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_convert_ac3ita_aac), curelem->opt.want_convert_ac3ita_aac);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_keep_ac3), curelem->opt.want_keep_ac3);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_keep_dolby), curelem->opt.want_keep_dolby);

		assert(curelem->destitems.size() < INT_MAX);
		setnewtreeviewmodel(int(curelem->destitems.size()));

		gtk_entry_set_text(GTK_ENTRY(entry_outname), curelem->outname.c_str());
	}
	
	return 0;
}

int dialogproperty_t::cancel() {
	gtk_label_set_text(GTK_LABEL(label_name), "");
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_item), NULL);
	gtk_entry_set_text(GTK_ENTRY(entry_outname), "");

	curn = -1;
	curelem = nullptr;

	return 0;
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

void dialogproperty_t::cb_keep_ac3(GtkToggleButton* toggle, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	
	for (destitem_t& item : inst->curelem->destitems) {
		if (item.type == itemtype_audio &&
		    item.codecid == codecid_ac3)

			item.want = gtk_toggle_button_get_active(toggle);
	}
	
	gtk_tree_model_filter_refilter(
	  GTK_TREE_MODEL_FILTER(
	    gtk_tree_view_get_model(
	      GTK_TREE_VIEW(inst->treeview_item))));
}

void dialogproperty_t::cb_keep_dolby(GtkToggleButton* toggle, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	
	for (destitem_t& item : inst->curelem->destitems) {
		if (item.type == itemtype_audio &&
		    item.codecid == codecid_dolby)

			item.want = gtk_toggle_button_get_active(toggle);
	}
	
	gtk_tree_model_filter_refilter(
	  GTK_TREE_MODEL_FILTER(
	    gtk_tree_view_get_model(
	      GTK_TREE_VIEW(inst->treeview_item))));
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

dialogproperty_t::~dialogproperty_t() {
	g_object_unref(builder);
}

void dialogproperty_t::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	assert(dialog == inst->dialog);

	delete inst;
}

static
void internal_cell_color(GtkCellRenderer *ren, gint n) {
	g_object_set(ren, "cell-background-set", (n%2) ? TRUE : FALSE, NULL);
}

void dialogproperty_t::cell_data_number(GtkTreeViewColumn *,
	                                    GtkCellRenderer *ren,
	                                    GtkTreeModel *filter_model,
	                                    GtkTreeIter *iter,
	                                    gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);

	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	g_object_set(ren, "text", self->curelem->destitems[n].num.c_str(), NULL);
	internal_cell_color(ren, n);
}

void dialogproperty_t::cell_data_name(GtkTreeViewColumn *,
	                                  GtkCellRenderer *ren,
	                                  GtkTreeModel *filter_model,
	                                  GtkTreeIter *iter,
	                                  gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	g_object_set(ren, "text", self->curelem->destitems[n].name.c_str(), NULL);
	internal_cell_color(ren, n);
}

void dialogproperty_t::cell_data_type(GtkTreeViewColumn *,
	                                  GtkCellRenderer *ren,
	                                  GtkTreeModel *filter_model,
	                                  GtkTreeIter *iter,
	                                  gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	const char *text = "";
	switch (self->curelem->destitems[n].type) {
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
	internal_cell_color(ren, n);
}

void dialogproperty_t::cell_data_codec(GtkTreeViewColumn *,
	                                   GtkCellRenderer *ren,
	                                   GtkTreeModel *filter_model,
	                                   GtkTreeIter *iter,
	                                   gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	g_object_set(ren, "text", self->curelem->destitems[n].codecname.c_str(), NULL);
	internal_cell_color(ren, n);
}

void dialogproperty_t::cell_data_language(GtkTreeViewColumn *,
	                                      GtkCellRenderer *ren,
	                                      GtkTreeModel *filter_model,
	                                      GtkTreeIter *iter,
	                                      gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	g_object_set(ren, "text", self->curelem->destitems[n].lang.c_str(), NULL);
	internal_cell_color(ren, n);
}

void dialogproperty_t::cell_data_isdefault(GtkTreeViewColumn *,
	                                       GtkCellRenderer *ren,
	                                       GtkTreeModel *filter_model,
	                                       GtkTreeIter *iter,
	                                       gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	gboolean bstate = self->curelem->destitems[n].isdefault;
	
	g_object_set(ren, "active", bstate, NULL);
	internal_cell_color(ren, n);
}

void dialogproperty_t::cell_data_isforced(GtkTreeViewColumn *,
	                                      GtkCellRenderer *ren,
	                                      GtkTreeModel *filter_model,
	                                      GtkTreeIter *iter,
	                                      gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;

	gboolean bstate = self->curelem->destitems[n].isforced;

	g_object_set(ren, "active", bstate, NULL);
	internal_cell_color(ren, n);
}

