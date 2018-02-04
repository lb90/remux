#include <cstdlib>
#include <cassert>
#include <utility>
#include <algorithm>
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
	check_remove_ac3     = GTK_WIDGET(gtk_builder_get_object(builder, "check_remove_ac3"));
	check_remove_dolby   = GTK_WIDGET(gtk_builder_get_object(builder, "check_remove_dolby"));
	treeview_item        = GTK_WIDGET(gtk_builder_get_object(builder, "treeview_item"));
	entry_outname        = GTK_WIDGET(gtk_builder_get_object(builder, "entry_outname"));
	textview_error       = GTK_WIDGET(gtk_builder_get_object(builder, "textview_error"));
	button_enqueue       = GTK_WIDGET(gtk_builder_get_object(builder, "button_enqueue"));
	button_skip          = GTK_WIDGET(gtk_builder_get_object(builder, "button_skip"));
	button_op            = GTK_WIDGET(gtk_builder_get_object(builder, "button_op"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
	g_signal_connect(button_enqueue, "clicked", G_CALLBACK(cb_enqueue), (gpointer) this);
	g_signal_connect(button_skip, "clicked", G_CALLBACK(cb_skip), (gpointer) this);
	g_signal_connect(button_op, "clicked", G_CALLBACK(cb_op), (gpointer) this);
	
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
	g_signal_connect(ren, "edited", G_CALLBACK(cb_edited_codec), (gpointer) this);
	ren = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "ren_language"));
	g_signal_connect(ren, "edited", G_CALLBACK(cb_edited_language), (gpointer) this);
	ren = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "ren_isdefault"));
	g_signal_connect(ren, "toggled", G_CALLBACK(cb_toggled_isdefault), (gpointer) this);
	ren = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "ren_isforced"));
	g_signal_connect(ren, "toggled", G_CALLBACK(cb_toggled_isforced), (gpointer) this);
	
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
	curn = n;
	
	treeview_select_n(curn);
	
	if (!curelem->isinit)
		op::media_scan(n);
	
	gtk_label_set_text(GTK_LABEL(label_name), curelem->name.c_str());

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
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview_item), NULL);
	gtk_entry_set_text(GTK_ENTRY(entry_outname), "");

	curn = -1;
	curelem = nullptr;

	return 0;
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

	std::vector<destitem_t> videoitems;
	std::vector<destitem_t> nonvideoitems;
	
	for (const destitem_t& item : elem.destitems) {
		if (item.type == itemtype_video)
			videoitems.emplace_back(item);
		else
			nonvideoitems.emplace_back(item);
	}
	
	for (const destitem_t& item : videoitems) {
		item.lang = "ita";
	}
	
	if (want_convert_ac3ita_aac) {
		std::vector<destitem_t> ac3itaitems;
		std::vector<destitem_t> nonac3itaitems;
		for (const destitem_t& item : nonvideoitems) {
			if (item.type == itemtype_audio && item.lang == "ita" &&
			      (item.codecid == codecid_ac3 || item.codecid == codecid_eac3))
				ac3itaitems.emplace_back(item);
			else
				nonac3itaitems.emplace_back(item);
		}
		
		for (destitem_t& ac3itaitem : ac3itaitems) {
			ac3itaitem.codecid = codecid_aac;
			ac3itaitem.isdefault = true;
			ac3itaitem.isforced = true;
		}
		
		nonvideoitems.clear();
		
		for (destitem_t& item : ac3itaitems)
			nonvideoitems.emplace_back(item);

		for (destitem_t& item : nonac3itaitems)
			nonvideoitems.emplace_back(item);
	}
	
	elem.destitems.clear();
	
	for (destitem_t& item : videoitems)
		elem.destitems.emplace_back(item);

	for (destitem_t& item : nonvideoitems)
		elem.destitems.emplace_back(item);
	
	for (destitem_t& item : elem.destitems) {
		if (item.type == itemtype_audio) {
			if (item.codecid == codecid_ac3 ||
			    item.codecid == codecid_eac3)
			{
				if (want_remove_ac3) {
					item.want = false;
				}
				else {
					item.isdefault = false;
					item.isforced = false;
				}
			}
		}
	}
	
	for (destitem_t& item : elem.destitems) {
		if (item.type == itemtype_audio) {
			if (item.codecid == codecid_dts ||
			    item.codecid == codecid_truehd)
			{
				if (want_remove_dolby) {
					item.want = false;
				}
				else {
					item.isdefault = false;
					item.isforced = false;
				}
			}
		}
	}
	
	for (destitem_t& item : elem.destitems) {
		if (item.type == itemtype_subtitle) {
			if (item.lang == "ita") {
				if (item.name.find("FORCED") != std::string::npos) {
					item.isdefault = true;
					item.isforced = true;
				}
			}
		}
	}
	
	gtk_tree_model_filter_refilter(
	  GTK_TREE_MODEL_FILTER(
	    gtk_tree_view_get_model(
	      GTK_TREE_VIEW(inst->treeview_item))));
}

void dialogproperty_t::cb_reset(GtkButton *, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	assert(inst && inst->curelem);
	
	media_t& elem = *(inst->curelem);
	
	elem.destitems.clear();
	
	for (const origitem_t& origitem : elem.origitems) {
		elem.destitems.emplace_back(origitem);
	}
}

void dialogproperty_t::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	assert(dialog == inst->dialog);

	delete inst;
}

dialogproperty_t::~dialogproperty_t() {
	g_object_unref(builder);
}







void dialogproperty_t::cb_edited_name(GtkCellRendererText *ren,
                                      gchar               *pathstr,
                                      gchar               *new_text,
                                      gpointer             self)
{
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
		
		inst->curelem->destitems[n].name = new_text;

		gtk_tree_path_free(path);
		path = NULL;
	}
}

void dialogproperty_t::cb_edited_codec(GtkCellRendererText *ren,
                                       gchar               *pathstr,
                                       gchar               *new_text,
                                       gpointer             self)
{
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
		
		std::string newtext = new_text;
		if (newtext == "AC-3")
			inst->curelem->destitems[n].codecid = codecid_ac3;
		else if (newtext == "EAC-3")
			inst->curelem->destitems[n].codecid = codecid_eac3;
		else if (newtext == "DTS")
			inst->curelem->destitems[n].codecid = codecid_dts;
		else if (newtext == "TrueHD")
			inst->curelem->destitems[n].codecid = codecid_truehd;
		else if (newtext == "AAC")
			inst->curelem->destitems[n].codecid = codecid_aac;
		else if (newtext == "MP2")
			inst->curelem->destitems[n].codecid = codecid_mp2;
		else if (newtext == "MP3")
			inst->curelem->destitems[n].codecid = codecid_mp3;
		else if (newtext == "Vorbis")
			inst->curelem->destitems[n].codecid = codecid_vorbis;
		else if (newtext == "FLAC")
			inst->curelem->destitems[n].codecid = codecid_flac;
		else if (newtext == "PCM")
			inst->curelem->destitems[n].codecid = codecid_pcm;
		else
			inst->curelem->destitems[n].codecid = codecid_other;

		gtk_tree_path_free(path);
		path = NULL;
	}
}

void dialogproperty_t::cb_edited_language(GtkCellRendererText *ren,
                                          gchar               *pathstr,
                                          gchar               *new_text,
                                          gpointer             self)
{
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
		
		inst->curelem->destitems[n].lang = new_text;

		gtk_tree_path_free(path);
		path = NULL;
	}
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
	                                    GtkTreeIter *iter,
	                                    gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	const destitem_t& item = self->curelem->destitems[n];

	g_object_set(ren, "text", std::to_string(n+1).c_str(), NULL);

	internal_cell_color(ren, item.type);
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
	                                  GtkTreeIter *iter,
	                                  gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);
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
	                                   GtkTreeIter *iter,
	                                   gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);
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
	                                      GtkTreeIter *iter,
	                                      gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);
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
	                                       GtkTreeIter *iter,
	                                       gpointer inst)
{
	GtkTreeIter it;
	gtk_tree_model_filter_convert_iter_to_child_iter(GTK_TREE_MODEL_FILTER(filter_model),
	                                                 &it,
	                                                 iter);
	gint n = GPOINTER_TO_INT(it.user_data);
	dialogproperty_t *self = (dialogproperty_t*) inst;
	const destitem_t& item = self->curelem->destitems[n];
	gboolean bstate = item.isdefault;
	
	g_object_set(ren, "active", bstate, NULL);
	internal_cell_color(ren, item.type);
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
	const destitem_t& item = self->curelem->destitems[n];
	gboolean bstate = item.isforced;

	g_object_set(ren, "active", bstate, NULL);
	internal_cell_color(ren, item.type);
}

gboolean dialogproperty_t::filter_visible_func(GtkTreeModel *childmodel, GtkTreeIter *iter, gpointer self) {
	dialogproperty_t *inst = (dialogproperty_t*) self;
	int n = GPOINTER_TO_INT(iter->user_data);
	
	if (n < 0 || size_t(n) >= inst->curelem->destitems.size())
		return TRUE;
	
	return inst->curelem->destitems[n].want;
}

