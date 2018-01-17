#include "treeviewmodel.h"
#include <cassert>
#include <vector>
#include <memory>
#include "elemnt.h"

extern std::vector<std::shared_ptr<onebigstruct>> model;

/*
 * Based on the work by Tim-Philip Muller about GtkTreeView.
*/

/* boring declarations of local functions */

static void              custom_model_init               (CustomModel *pkg_tree);
static void              custom_model_class_init         (CustomModelClass *klass);
static void              custom_model_tree_model_init    (GtkTreeModelIface *iface);
static void              custom_model_finalize           (GObject *object);
static GtkTreeModelFlags custom_model_get_flags          (GtkTreeModel *tree_model);
static gint              custom_model_get_n_columns      (GtkTreeModel *tree_model);
static GType             custom_model_get_column_type    (GtkTreeModel *tree_model, gint index);
static gboolean          custom_model_get_iter           (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreePath *path);
static GtkTreePath *     custom_model_get_path           (GtkTreeModel *tree_model, GtkTreeIter *iter);
static void              custom_model_get_value          (GtkTreeModel *tree_model, GtkTreeIter *iter, gint column, GValue *value);
static gboolean          custom_model_iter_next          (GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean          custom_model_iter_children      (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent);
static gboolean          custom_model_iter_has_child     (GtkTreeModel *tree_model, GtkTreeIter *iter);
static gint              custom_model_iter_n_children    (GtkTreeModel *tree_model, GtkTreeIter *iter);
static gboolean          custom_model_iter_nth_child     (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *parent, gint n);
static gboolean          custom_model_iter_parent        (GtkTreeModel *tree_model, GtkTreeIter *iter, GtkTreeIter *child);


static GObjectClass *parent_class = NULL;  /* GObject stuff - nothing to worry about */


/*****************************************************************************
 *
 *  custom_list_get_type: here we register our new type and its interfaces
 *                        with the type system. If you want to implement
 *                        additional interfaces like GtkTreeSortable, you
 *                        will need to do it here.
 *
 *****************************************************************************/

GType custom_model_get_type(void) {
	static GType custommodel_type = 0;

	/* Some boilerplate type registration stuff */
	if (custommodel_type == 0)
	{
		static const GTypeInfo custommodel_info = {
			sizeof (CustomModelClass),
			NULL,                                         /* base_init */
			NULL,                                         /* base_finalize */
			(GClassInitFunc) custom_model_class_init,
			NULL,                                         /* class finalize */
			NULL,                                         /* class_data */
			sizeof (CustomModel),
			0,                                            /* n_preallocs */
			(GInstanceInitFunc) custom_model_init
		};

		static const GInterfaceInfo treemodel_info = {
			(GInterfaceInitFunc) custom_model_tree_model_init,
			NULL,
			NULL
		};

		/* First register the new derived type with the GObject type system */
		custommodel_type = g_type_register_static(G_TYPE_OBJECT, "CustomModel", &custommodel_info, (GTypeFlags) 0);

		/* Now register our GtkTreeModel interface with the type system */
		g_type_add_interface_static(custommodel_type, GTK_TYPE_TREE_MODEL, &treemodel_info);
	}

	return custommodel_type;
}


/*****************************************************************************
 *
 *  custom_list_class_init: more boilerplate GObject/GType stuff.
 *                          Init callback for the type system,
 *                          called once when our new class is created.
 *
 *****************************************************************************/

static void
custom_model_class_init (CustomModelClass *klass)
{
  GObjectClass *object_class;

  parent_class = (GObjectClass*) g_type_class_peek_parent (klass);
  object_class = (GObjectClass*) klass;

  object_class->finalize = custom_model_finalize;
}

/*****************************************************************************
 *
 *  custom_list_tree_model_init: init callback for the interface registration
 *                               in custom_list_get_type. Here we override
 *                               the GtkTreeModel interface functions that
 *                               we implement.
 *
 *****************************************************************************/

static
void custom_model_tree_model_init(GtkTreeModelIface *iface) {
	iface->get_flags       = custom_model_get_flags;
	iface->get_n_columns   = custom_model_get_n_columns;
	iface->get_column_type = custom_model_get_column_type;
	iface->get_iter        = custom_model_get_iter;
	iface->get_path        = custom_model_get_path;
	iface->get_value       = custom_model_get_value;
	iface->iter_next       = custom_model_iter_next;
	iface->iter_children   = custom_model_iter_children;
	iface->iter_has_child  = custom_model_iter_has_child;
	iface->iter_n_children = custom_model_iter_n_children;
	iface->iter_nth_child  = custom_model_iter_nth_child;
	iface->iter_parent     = custom_model_iter_parent;
}


/*****************************************************************************
 *
 *  custom_list_init: this is called everytime a new custom list object
 *                    instance is created (we do that in custom_list_new).
 *                    Initialise the list structure's fields here.
 *
 *****************************************************************************/

static
void custom_model_init(CustomModel *custommodel) {
	custommodel->stamp = g_random_int();  /* Random int to check whether an iter belongs to our model */
}


/*****************************************************************************
 *
 *  custom_list_finalize: this is called just before a custom list is
 *                        destroyed. Free dynamically allocated memory here.
 *
 *****************************************************************************/

static
void custom_model_finalize(GObject *object) {
	/* we have nothing to free as the model is an interafce adaptor to our application model */
	(* parent_class->finalize) (object);
}


/*****************************************************************************
 *
 *  custom_list_get_flags: tells the rest of the world whether our tree model
 *                         has any special characteristics. In our case,
 *                         we have a list model (instead of a tree), and each
 *                         tree iter is valid as long as the row in question
 *                         exists, as it only contains a pointer to our struct.
 *
 *****************************************************************************/

static
GtkTreeModelFlags custom_model_get_flags(GtkTreeModel *treemodel) {
	return (GtkTreeModelFlags) (GTK_TREE_MODEL_LIST_ONLY | GTK_TREE_MODEL_ITERS_PERSIST);
}


/*****************************************************************************
 *
 *  custom_list_get_n_columns: tells the rest of the world how many data
 *                             columns we export via the tree model interface
 *
 *****************************************************************************/

/* number of model attributes */

static
gint custom_model_get_n_columns(GtkTreeModel *treemodel) {
	return N_ATTRIBUTES;
}


/*****************************************************************************
 *
 *  custom_list_get_column_type: tells the rest of the world which type of
 *                               data an exported model column contains
 *
 *****************************************************************************/

static
GType custom_model_get_column_type(GtkTreeModel *treemodel, gint index) {
	g_return_val_if_fail (index >= 0 && index < N_ATTRIBUTES, G_TYPE_INVALID);

	return G_TYPE_POINTER;
}


/*****************************************************************************
 *
 *  custom_list_get_iter: converts a tree path (physical position) into a
 *                        tree iter structure (the content of the iter
 *                        fields will only be used internally by our model).
 *                        We simply store a pointer to our CustomRecord
 *                        structure that represents that row in the tree iter.
 *
 *****************************************************************************/

static
gboolean custom_model_get_iter(GtkTreeModel *treemodel, GtkTreeIter *iter, GtkTreePath *path) {
	g_assert(path != NULL);

	CustomModel *custommodel = CUSTOM_MODEL(treemodel);

	gint *indices, depth;
	indices = gtk_tree_path_get_indices(path);
	depth   = gtk_tree_path_get_depth(path);

	/* we do not allow children */
	g_assert(depth == 1); /* depth 1 = top level; a list only has top level nodes and no children */

	gint n = indices[0]; /* the n-th row */

	if (n >= model.size() || n < 0)
		return FALSE;

	iter->stamp      = custommodel->stamp;
	iter->user_data  = (gpointer) (intptr_t) n;
	iter->user_data2 = NULL; /* unused */
	iter->user_data3 = NULL; /* unused */

	return TRUE;
}


/*****************************************************************************
 *
 *  custom_list_get_path: converts a tree iter into a tree path (ie. the
 *                        physical position of that row in the list).
 *
 *****************************************************************************/

static
GtkTreePath* custom_model_get_path(GtkTreeModel *treemodel, GtkTreeIter *iter) {
	if (iter == NULL)
		return NULL;

	gint n = (gint) (intptr_t) iter->user_data;
	
	if (n < 0 || n >= model.size())
		return NULL;

	GtkTreePath  *path = gtk_tree_path_new();
	gtk_tree_path_append_index(path, n);

	return path;
}


/*****************************************************************************
 *
 *  custom_list_get_value: Returns a row's exported data columns
 *                         (_get_value is what gtk_tree_model_get uses)
 *
 *****************************************************************************/

static
void custom_model_get_value(GtkTreeModel *treemodel, GtkTreeIter *iter, gint column, GValue *value) {
	g_return_if_fail(iter != NULL);
	g_return_if_fail(column < N_ATTRIBUTES);

	g_value_init(value, G_TYPE_POINTER);

	onebigstruct *obs = NULL;
	gint n = (gint) (intptr_t) iter->user_data;
	g_return_if_fail(n >= 0 && n < model.size());
	obs = model[n].get();
	g_return_if_fail(obs != NULL);

	g_value_set_pointer(value, obs);
}


/*****************************************************************************
 *
 *  custom_list_iter_next: Takes an iter structure and sets it to point
 *                         to the next row.
 *
 *****************************************************************************/

static
gboolean custom_model_iter_next(GtkTreeModel *treemodel, GtkTreeIter *iter) {
	CustomModel *custommodel;

	if (iter == NULL) return FALSE;

    custommodel = CUSTOM_MODEL(treemodel);

	gint n = (gint) (intptr_t) iter->user_data;
	if (n < 0) return FALSE;
	if (n >= model.size()) return FALSE;
	
	if (n == model.size() - 1)
		return FALSE;

	iter->stamp = custommodel->stamp;
	iter->user_data = (gpointer) (intptr_t) (n + 1);

	return TRUE;
}


/*****************************************************************************
 *
 *  custom_list_iter_children: Returns TRUE or FALSE depending on whether
 *                             the row specified by 'parent' has any children.
 *                             If it has children, then 'iter' is set to
 *                             point to the first child. Special case: if
 *                             'parent' is NULL, then the first top-level
 *                             row should be returned if it exists.
 *
 *****************************************************************************/

static gboolean
custom_model_iter_children (GtkTreeModel *treemodel, GtkTreeIter *iter, GtkTreeIter *parent) {
	CustomModel  *custommodel;

	/* this is a list, nodes have no children */
	if (parent)
		return FALSE;

	/* parent == NULL is a special case; we need to return the first top-level row */

	custommodel = CUSTOM_MODEL(treemodel);

	/* No rows => no first row */
	if (model.size() == 0) return FALSE;

	/* Set iter to first item in list */
	iter->stamp     = custommodel->stamp;
	iter->user_data = (gpointer) 0;

	return TRUE;
}


/*****************************************************************************
 *
 *  custom_list_iter_has_child: Returns TRUE or FALSE depending on whether
 *                              the row specified by 'iter' has any children.
 *                              We only have a list and thus no children.
 *
 *****************************************************************************/

static gboolean
custom_model_iter_has_child(GtkTreeModel *treemodel, GtkTreeIter *iter)
{
  return FALSE;
}


/*****************************************************************************
 *
 *  custom_list_iter_n_children: Returns the number of children the row
 *                               specified by 'iter' has. This is usually 0,
 *                               as we only have a list and thus do not have
 *                               any children to any rows. A special case is
 *                               when 'iter' is NULL, in which case we need
 *                               to return the number of top-level nodes,
 *                               ie. the number of rows in our list.
 *
 *****************************************************************************/

static gint
custom_model_iter_n_children(GtkTreeModel *treemodel, GtkTreeIter *iter)
{
	CustomModel *custommodel;

	custommodel = CUSTOM_MODEL(treemodel);

	/* special case: if iter == NULL, return number of top-level rows */
	if (!iter)
		return model.size();

	return 0; /* otherwise, this is easy again for a list */
}


/*****************************************************************************
 *
 *  custom_list_iter_nth_child: If the row specified by 'parent' has any
 *                              children, set 'iter' to the n-th child and
 *                              return TRUE if it exists, otherwise FALSE.
 *                              A special case is when 'parent' is NULL, in
 *                              which case we need to set 'iter' to the n-th
 *                              row if it exists.
 *
 *****************************************************************************/

static gboolean
custom_model_iter_nth_child(GtkTreeModel *treemodel, GtkTreeIter *iter, GtkTreeIter *parent, gint n) {
	CustomModel *custommodel;

	custommodel = CUSTOM_MODEL(treemodel);

	/* a list has only top-level rows */
	if (parent)
		return FALSE;

	/* special case: if parent == NULL, set iter to n-th top-level row */

	if (n >= model.size())
		return FALSE;

	iter->stamp = custommodel->stamp;
	iter->user_data = (gpointer) (intptr_t) n;

	return TRUE;
}


/*****************************************************************************
 *
 *  custom_list_iter_parent: Point 'iter' to the parent node of 'child'. As
 *                           we have a list and thus no children and no
 *                           parents of children, we can just return FALSE.
 *
 *****************************************************************************/

static gboolean
custom_model_iter_parent(GtkTreeModel *treemodel, GtkTreeIter *iter, GtkTreeIter *child) {
	return FALSE;
}


/*****************************************************************************
 *
 *  custom_list_new:  This is what you use in your own code to create a
 *                    new custom list tree model for you to use.
 *
 *****************************************************************************/

CustomModel* custom_model_new(void) {
	CustomModel *newcustommodel;

	newcustommodel = (CustomModel*) g_object_new(CUSTOM_TYPE_MODEL, NULL);

	g_assert(newcustommodel != NULL);

	return newcustommodel;
}

void custom_model_record_appended(CustomModel *custommodel) {
	GtkTreeIter   iter;
	GtkTreePath  *path;
	
	if (model.size() == 0) return;
	
	gint n = model.size() - 1;

	path = gtk_tree_path_new();
	gtk_tree_path_append_index(path, n);
	custom_model_get_iter(GTK_TREE_MODEL(custommodel), &iter, path);
	gtk_tree_model_row_inserted(GTK_TREE_MODEL(custommodel), path, &iter);
	gtk_tree_path_free(path);
}

void custom_model_record_changed(CustomModel *custommodel, int n) {
	assert(n >= 0);
	assert(n < model.size());
	
	GtkTreeIter   iter;
	GtkTreePath  *path;
	
	path = gtk_tree_path_new();
	gtk_tree_path_append_index(path, n);
	custom_model_get_iter(GTK_TREE_MODEL(custommodel), &iter, path);
	gtk_tree_model_row_changed(GTK_TREE_MODEL(custommodel), path, &iter);
	gtk_tree_path_free(path);
}

void custom_model_record_removed(CustomModel *custommodel, int n) {
	assert(n >= 0);
	assert(n <= model.size());

	GtkTreePath  *path;
	
	path = gtk_tree_path_new();
	gtk_tree_path_append_index(path, n);
	gtk_tree_model_row_deleted(GTK_TREE_MODEL(custommodel), path);
	gtk_tree_path_free(path);
}

