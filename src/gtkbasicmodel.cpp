#include <cstdlib>
#include <gtk/gtk.h>
#include "gtkbasicmodel.h"

/* boring declarations of local functions */

static void         basic_list_model_init            (BasicListModel      *pkg_tree);

static void         basic_list_model_class_init      (BasicListModelClass *klass);

static void         basic_list_model_tree_model_init (GtkTreeModelIface *iface);

static void         basic_list_model_finalize        (GObject           *object);

static GtkTreeModelFlags basic_list_model_get_flags  (GtkTreeModel      *tree_model);

static gint         basic_list_model_get_n_columns   (GtkTreeModel      *tree_model);

static GType        basic_list_model_get_column_type (GtkTreeModel      *tree_model,
                                                      gint               index);

static gboolean     basic_list_model_get_iter        (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter,
                                                      GtkTreePath       *path);

static GtkTreePath *basic_list_model_get_path        (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter);

static void         basic_list_model_get_value       (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter,
                                                      gint               column,
                                                      GValue            *value);

static gboolean     basic_list_model_iter_next       (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter);

static gboolean     basic_list_model_iter_children   (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter,
                                                      GtkTreeIter       *parent);

static gboolean     basic_list_model_iter_has_child  (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter);

static gint         basic_list_model_iter_n_children (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter);

static gboolean     basic_list_model_iter_nth_child  (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter,
                                                      GtkTreeIter       *parent,
                                                      gint               n);

static gboolean     basic_list_model_iter_parent     (GtkTreeModel      *tree_model,
                                                      GtkTreeIter       *iter,
                                                      GtkTreeIter       *child);



static GObjectClass *parent_class = NULL;  /* GObject stuff - nothing to worry about */


/*****************************************************************************
 *
 *  basic_list_model_get_type: here we register our new type and its interfaces
 *                             with the type system. If you want to implement
 *                             additional interfaces like GtkTreeSortable, you
 *                             will need to do it here.
 *
 *****************************************************************************/

GType
basic_list_model_get_type (void)
{
  static GType basic_list_model_type = 0;

  /* Some boilerplate type registration stuff */
  if (basic_list_model_type == 0)
  {
    static const GTypeInfo basic_list_model_info =
    {
      sizeof (BasicListModelClass),
      NULL,                                         /* base_init */
      NULL,                                         /* base_finalize */
      (GClassInitFunc) basic_list_model_class_init,
      NULL,                                         /* class finalize */
      NULL,                                         /* class_data */
      sizeof (BasicListModel),
      0,                                           /* n_preallocs */
      (GInstanceInitFunc) basic_list_model_init
    };
    static const GInterfaceInfo tree_model_info =
    {
      (GInterfaceInitFunc) basic_list_model_tree_model_init,
      NULL,
      NULL
    };

    /* First register the new derived type with the GObject type system */
    basic_list_model_type = g_type_register_static (G_TYPE_OBJECT, "BasicListModel",
                                                   &basic_list_model_info, (GTypeFlags)0);

    /* Now register our GtkTreeModel interface with the type system */
    g_type_add_interface_static (basic_list_model_type, GTK_TYPE_TREE_MODEL, &tree_model_info);
  }

  return basic_list_model_type;
}


/*****************************************************************************
 *
 *  basic_list_model_class_init: more boilerplate GObject/GType stuff.
 *                               Init callback for the type system,
 *                               called once when our new class is created.
 *
 *****************************************************************************/

static void
basic_list_model_class_init (BasicListModelClass *klass)
{
  GObjectClass *object_class;

  parent_class = (GObjectClass*) g_type_class_peek_parent (klass);
  object_class = (GObjectClass*) klass;

  object_class->finalize = basic_list_model_finalize;
}

/*****************************************************************************
 *
 *  basic_list_model_tree_model_init: init callback for the interface registration
 *                                    in custom_list_get_type. Here we override
 *                                    the GtkTreeModel interface functions that
 *                                    we implement.
 *
 *****************************************************************************/

static void
basic_list_model_tree_model_init (GtkTreeModelIface *iface)
{
  iface->get_flags       = basic_list_model_get_flags;
  iface->get_n_columns   = basic_list_model_get_n_columns;
  iface->get_column_type = basic_list_model_get_column_type;
  iface->get_iter        = basic_list_model_get_iter;
  iface->get_path        = basic_list_model_get_path;
  iface->get_value       = basic_list_model_get_value;
  iface->iter_next       = basic_list_model_iter_next;
  iface->iter_children   = basic_list_model_iter_children;
  iface->iter_has_child  = basic_list_model_iter_has_child;
  iface->iter_n_children = basic_list_model_iter_n_children;
  iface->iter_nth_child  = basic_list_model_iter_nth_child;
  iface->iter_parent     = basic_list_model_iter_parent;
}


/*****************************************************************************
 *
 *  basic_list_model_init: this is called everytime a new custom list object
 *                         instance is created (we do that in basic_list_model_new).
 *                         Initialise the list structure's fields here.
 *
 *****************************************************************************/

static void
basic_list_model_init (BasicListModel *basic_list_model)
{
  basic_list_model->stamp   = g_random_int();  /* Random int to check whether an iter belongs to our model */
  basic_list_model->numrows = 0;
}


/*****************************************************************************
 *
 *  basic_list_model_finalize: this is called just before a custom list is
 *                             destroyed. Free dynamically allocated memory here.
 *
 *****************************************************************************/

static void
basic_list_model_finalize (GObject *object)
{
  /* must chain up - finalize parent */
  (* parent_class->finalize) (object);
}


/*****************************************************************************
 *
 *  basic_list_model_get_flags: tells the rest of the world whether our tree model
 *                              has any special characteristics. In our case,
 *                              we have a list model (instead of a tree), and each
 *                              tree iter is valid as long as the row in question
 *                              exists, as it only contains a pointer to our struct.
 *
 *****************************************************************************/

static GtkTreeModelFlags
basic_list_model_get_flags (GtkTreeModel *tree_model)
{
  g_assert(IS_BASIC_LIST_MODEL(tree_model));

  return (GtkTreeModelFlags) GTK_TREE_MODEL_LIST_ONLY;
}


/*****************************************************************************
 *
 *  basic_list_model_get_n_columns: tells the rest of the world how many data
 *                                  columns we export via the tree model interface
 *
 *****************************************************************************/

static gint
basic_list_model_get_n_columns (GtkTreeModel *tree_model)
{
  g_assert(IS_BASIC_LIST_MODEL(tree_model));

  return 1;
}


/*****************************************************************************
 *
 *  basic_list_model_get_column_type: tells the rest of the world which type of
 *                                    data an exported model column contains
 *
 *****************************************************************************/

static GType
basic_list_model_get_column_type (GtkTreeModel *tree_model,
                                  gint          index)
{
  g_return_val_if_fail (IS_BASIC_LIST_MODEL(tree_model), G_TYPE_INVALID);
  g_return_val_if_fail (index == 0, G_TYPE_INVALID);

  return G_TYPE_POINTER;
}


/*****************************************************************************
 *
 *  basic_list_model_get_iter: converts a tree path (physical position) into a
 *                             tree iter structure (the content of the iter
 *                             fields will only be used internally by our model).
 *
 *****************************************************************************/

static gboolean
basic_list_model_get_iter (GtkTreeModel *tree_model,
                           GtkTreeIter  *iter,
                           GtkTreePath  *path)
{
  BasicListModel *basic_list_model;
  gint           *indices, depth;
  gint            n;

  g_assert(IS_BASIC_LIST_MODEL(tree_model));
  g_assert(path!=NULL);

  basic_list_model = BASIC_LIST_MODEL(tree_model);

  indices = gtk_tree_path_get_indices(path);
  depth   = gtk_tree_path_get_depth(path);

  g_assert(depth == 1);
  n = indices[0];
  
  g_return_val_if_fail( n < basic_list_model->numrows, FALSE );

  /*TODO can we just do it? is it ok with persistent  iters?*/
  /* and if we use a list, can we store an iterator? */
  iter->stamp      = basic_list_model->stamp;
  iter->user_data  = GINT_TO_POINTER(n);
  iter->user_data2 = NULL;   /* unused */
  iter->user_data3 = NULL;   /* unused */

  return TRUE;
}


/*****************************************************************************
 *
 *  basic_list_model_get_path: converts a tree iter into a tree path (ie. the
 *                             physical position of that row in the list).
 *
 *****************************************************************************/

static GtkTreePath *
basic_list_model_get_path (GtkTreeModel *tree_model,
                           GtkTreeIter  *iter)
{
  BasicListModel *basic_list_model;
  GtkTreePath    *path;
  gint            n;

  g_return_val_if_fail (IS_BASIC_LIST_MODEL(tree_model), NULL);
  basic_list_model = BASIC_LIST_MODEL(tree_model);

  g_return_val_if_fail (iter != NULL, NULL);

  n = GPOINTER_TO_INT(iter->user_data);
  g_return_val_if_fail( n < basic_list_model->numrows, NULL );

  path = gtk_tree_path_new();
  gtk_tree_path_append_index(path, n);

  return path;
}


/*****************************************************************************
 *
 *  basic_list_model_get_value: Returns a row's exported data columns
 *                              (_get_value is what gtk_tree_model_get uses)
 *
 *****************************************************************************/

static void
basic_list_model_get_value (GtkTreeModel *tree_model,
                            GtkTreeIter  *iter,
                            gint          column,
                            GValue       *value)
{
  BasicListModel    *basic_list_model;
  gint               n;
  
  value = NULL;

  g_return_if_fail (IS_BASIC_LIST_MODEL (tree_model));
  basic_list_model = BASIC_LIST_MODEL(tree_model);

  g_return_if_fail (iter != NULL);
  g_return_if_fail (column == 0); /* we only have one column */

  n = GPOINTER_TO_INT(iter->user_data);

  g_return_if_fail ( n < basic_list_model->numrows );
  
  g_value_init (value, G_TYPE_POINTER);
  g_value_set_pointer(value, NULL);
}


/*****************************************************************************
 *
 *  basic_list_model_iter_next: Takes an iter structure and sets it to point
 *                              to the next row (at the same level)
 *
 *****************************************************************************/

static gboolean
basic_list_model_iter_next (GtkTreeModel  *tree_model,
                            GtkTreeIter   *iter)
{
  BasicListModel    *basic_list_model;
  gint               n;

  g_return_val_if_fail (IS_BASIC_LIST_MODEL (tree_model), FALSE);
  basic_list_model = BASIC_LIST_MODEL(tree_model);

  if (iter == NULL)
    return FALSE;
  
  n = GPOINTER_TO_INT(iter->user_data);
  
  g_return_val_if_fail ( n + 1 < basic_list_model->numrows , FALSE );
  
  iter->stamp      = basic_list_model->stamp;
  iter->user_data  = GINT_TO_POINTER(n + 1);
  iter->user_data2 = NULL;   /* unused */
  iter->user_data3 = NULL;   /* unused */

  return TRUE;
}


/*****************************************************************************
 *
 *  basic_list_model_iter_children: Returns TRUE or FALSE depending on whether
 *                                  the row specified by 'parent' has any children.
 *                                  If it has children, then 'iter' is set to
 *                                  point to the first child. Special case: if
 *                                  'parent' is NULL, then the first top-level
 *                                  row should be returned if it exists.
 *
 *****************************************************************************/

static gboolean
basic_list_model_iter_children (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter,
                              GtkTreeIter  *parent)
{
  BasicListModel  *basic_list_model;
  
  g_return_val_if_fail (IS_BASIC_LIST_MODEL (tree_model), FALSE);
  basic_list_model = BASIC_LIST_MODEL(tree_model);

  if (parent == NULL) {
    if (basic_list_model->numrows > 0) {
      iter->stamp      = basic_list_model->stamp;
      iter->user_data  = GINT_TO_POINTER(0);
      iter->user_data2 = NULL;   /* unused */
      iter->user_data3 = NULL;   /* unused */

      return TRUE;
    }
  }
  
  return FALSE;
}


/*****************************************************************************
 *
 *  basic_list_model_iter_has_child: Returns TRUE or FALSE depending on whether
 *                                   the row specified by 'iter' has any children.
 *
 *****************************************************************************/

static gboolean
basic_list_model_iter_has_child (GtkTreeModel *tree_model,
                               GtkTreeIter  *iter)
{
  IS_BASIC_LIST_MODEL(tree_model);

  return FALSE;
}


/*****************************************************************************
 *
 *  basic_list_model_iter_n_children: Returns the number of children the row
 *                                    specified by 'iter' has. A special case is
 *                                    when 'iter' is NULL, in which case we need
 *                                    to return the number of top-level nodes.
 *
 *****************************************************************************/

static gint
basic_list_model_iter_n_children (GtkTreeModel *tree_model,
                                GtkTreeIter  *iter)
{
  BasicListModel  *basic_list_model;

  g_return_val_if_fail (IS_BASIC_LIST_MODEL (tree_model), -1);
  basic_list_model = BASIC_LIST_MODEL(tree_model);
  
  if (iter == NULL)
    return basic_list_model->numrows;
  else
    return 0;
}


/*****************************************************************************
 *
 *  basic_list_model_iter_nth_child: If the row specified by 'parent' has any
 *                                   children, set 'iter' to the n-th child and
 *                                   return TRUE if it exists, otherwise FALSE.
 *                                   A special case is when 'parent' is NULL, in
 *                                   which case we need to set 'iter' to the n-th
 *                                   row if it exists.
 *
 *****************************************************************************/

static gboolean
basic_list_model_iter_nth_child (GtkTreeModel *tree_model,
                               GtkTreeIter  *iter,
                               GtkTreeIter  *parent,
                               gint          n)
{
  BasicListModel    *basic_list_model;

  g_return_val_if_fail( IS_BASIC_LIST_MODEL(tree_model), FALSE );
  g_return_val_if_fail( n >= 0, FALSE );

  basic_list_model = BASIC_LIST_MODEL(tree_model);

  if (parent == NULL) {
    if ( n < basic_list_model->numrows ) {
      iter->stamp      = basic_list_model->stamp;
      iter->user_data  = GINT_TO_POINTER(n);
      iter->user_data2 = NULL;
      iter->user_data3 = NULL;
    }
    else
      return FALSE;
  }
  else
    return FALSE;
  
  return TRUE;
}


/*****************************************************************************
 *
 *  basic_list_iter_parent: Point 'iter' to the parent node of 'child'. As
 *                          we have a list and thus no children and no
 *                          parents of children, we can just return FALSE.
 *
 *****************************************************************************/

static gboolean
basic_list_model_iter_parent (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter,
                              GtkTreeIter  *child)
{
  IS_BASIC_LIST_MODEL(tree_model);
  
  return FALSE;
}

/*****************************************************************************
 *
 *  basic_list_model_new:  This is what you use in your own code to create a
 *                         new basic list tree model for you to use.
 *
 *****************************************************************************/

BasicListModel *
basic_list_model_new (gint numrows)
{
  BasicListModel *newbasicmodel;

  newbasicmodel = (BasicListModel*) g_object_new (TYPE_BASIC_LIST_MODEL, NULL);
  g_assert( newbasicmodel != NULL );
  
  newbasicmodel->numrows = numrows;

  return newbasicmodel;
}

