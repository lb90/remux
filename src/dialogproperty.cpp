#include <cstdlib>
#include <cassert>
#include <utility>
#include "dialogproperty.h"
#include "model.h"
#include "elements.h"
#include "op.h"

dialogproperty_t::dialogproperty_t(GtkWindow *window)
{
	builder = gtk_builder_new_from_resource("/org/remux/remux/ui/property.ui");
	
	dialog               = GTK_DIALOG(gtk_builder_get_object(builder, "dialog_property"));
	stack                = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));
	label_name           = GTK_WIDGET(gtk_builder_get_object(builder, "label_name"));
	check_convert_ac3ita_aac = GTK_WIDGET(gtk_builder_get_object(builder, "check_convert_ac3ita_aac"));
	check_leave_ac3      = GTK_WIDGET(gtk_builder_get_object(builder, "check_leave_ac3"));
	check_leave_dolby    = GTK_WIDGET(gtk_builder_get_object(builder, "check_leave_dolby"));
	treeview_item        = GTK_WIDGET(gtk_builder_get_object(builder, "treeview_item"));
	entry_outname        = GTK_WIDGET(gtk_builder_get_object(builder, "entry_outname"));
	textview_error       = GTK_WIDGET(gtk_builder_get_object(builder, "textview_error"));
	button_enqueue       = GTK_WIDGET(gtk_builder_get_object(builder, "button_enqueue"));
	button_skip          = GTK_WIDGET(gtk_builder_get_object(builder, "button_skip"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
	
	GtkTreeViewColumn *col;
	GtkCellRenderer   *ren;
	std::vector<std::pair<std::string, GtkTreeCellDataFunc>> colv = {
		std::make_pair("number",    cell_data_number),
		std::make_pair("name",      cell_data_name),
		std::make_pair("type",      cell_data_type),
		std::make_pair("format",    cell_data_format),
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
	
	curn = -1;
	curelem = nullptr;
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

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_convert_ac3ita_aac), curelem->opt.convert_ac3ita_aac);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_leave_ac3), curelem->opt.leave_ac3);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_leave_dolby), curelem->opt.leave_dolby);

		basic_model = basic_list_model_new(curelem->items.size());
		gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_item), GTK_TREE_MODEL(basic_model));

		gtk_entry_set_text(GTK_ENTRY(entry_outname), curelem->outname.c_str());
	}
	
	return 0;
}

int dialogproperty_t::cancel() {
/*	gtk_label_set_text(GTK_LABEL(label_name), "");
	gtk_*/

	curn = -1;
	curelem = nullptr;

	return 0;
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
	                                    GtkTreeModel *,
	                                    GtkTreeIter *iter,
	                                    gpointer inst)
{
	gint n = GPOINTER_TO_INT(iter->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	g_object_set(ren, "text", self->curelem->items[n].num.c_str(), NULL);
	internal_cell_color(ren, n);
}

void dialogproperty_t::cell_data_name(GtkTreeViewColumn *,
	                                  GtkCellRenderer *ren,
	                                  GtkTreeModel *,
	                                  GtkTreeIter *iter,
	                                  gpointer inst)
{
	gint n = GPOINTER_TO_INT(iter->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	g_object_set(ren, "text", self->curelem->items[n].name.c_str(), NULL);
	internal_cell_color(ren, n);
}

void dialogproperty_t::cell_data_type(GtkTreeViewColumn *,
	                                  GtkCellRenderer *ren,
	                                  GtkTreeModel *,
	                                  GtkTreeIter *iter,
	                                  gpointer inst)
{
	gint n = GPOINTER_TO_INT(iter->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	const char *text = "";
	switch (self->curelem->items[n].type) {
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

void dialogproperty_t::cell_data_format(GtkTreeViewColumn *,
	                                  GtkCellRenderer *ren,
	                                  GtkTreeModel *,
	                                  GtkTreeIter *iter,
	                                  gpointer inst)
{
	gint n = GPOINTER_TO_INT(iter->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	g_object_set(ren, "text", self->curelem->items[n].codecname.c_str(), NULL);
	internal_cell_color(ren, n);
}

void dialogproperty_t::cell_data_language(GtkTreeViewColumn *,
	                                      GtkCellRenderer *ren,
	                                      GtkTreeModel *,
	                                      GtkTreeIter *iter,
	                                      gpointer inst)
{
	gint n = GPOINTER_TO_INT(iter->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	g_object_set(ren, "text", self->curelem->items[n].lang.c_str(), NULL);
	internal_cell_color(ren, n);
}

void dialogproperty_t::cell_data_isdefault(GtkTreeViewColumn *,
	                                       GtkCellRenderer *ren,
	                                       GtkTreeModel *,
	                                       GtkTreeIter *iter,
	                                       gpointer inst)
{
	gint n = GPOINTER_TO_INT(iter->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	const char *text;
	if (self->curelem->items[n].want_default)
		text = "Sì";
	else
		text = "No";
	
	g_object_set(ren, "text", text, NULL);
	internal_cell_color(ren, n);
}

void dialogproperty_t::cell_data_isforced(GtkTreeViewColumn *,
	                                      GtkCellRenderer *ren,
	                                      GtkTreeModel *,
	                                      GtkTreeIter *iter,
	                                      gpointer inst)
{
	gint n = GPOINTER_TO_INT(iter->user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	
	const char *text;
	if (self->curelem->items[n].want_forced)
		text = "Sì";
	else
		text = "No";
	
	g_object_set(ren, "text", text, NULL);
	internal_cell_color(ren, n);
}

