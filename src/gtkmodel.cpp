#include <cstdlib>
#include <gtk/gtk.h>
#include "gtkmodel.h"

/* boring declarations of local functions */

static void         fake_list_model_init            (FakeListModel      *pkg_tree);

static void         fake_list_model_class_init      (FakeListModelClass *klass);

static void         fake_list_model_tree_model_init (GtkTreeModelIface *iface);

static void         fake_list_model_finalize        (GObject           *object);

static GtkTreeModelFlags fake_list_model_get_flags  (GtkTreeModel      *tree_model);

static gint         fake_list_model_get_n_columns   (GtkTreeModel      *tree_model);

static GType        fake_list_model_get_column_type (GtkTreeModel      *tree_model,
                                                     gint               index);

static gboolean     fake_list_model_get_iter        (GtkTreeModel      *tree_model,
                                                     GtkTreeIter       *iter,
                                                     GtkTreePath       *path);

static GtkTreePath *fake_list_model_get_path        (GtkTreeModel      *tree_model,
                                                     GtkTreeIter       *iter);

static void         fake_list_model_get_value       (GtkTreeModel      *tree_model,
                                                     GtkTreeIter       *iter,
                                                     gint               column,
                                                     GValue            *value);

static gboolean     fake_list_model_iter_next       (GtkTreeModel      *tree_model,
                                                     GtkTreeIter       *iter);

static gboolean     fake_list_model_iter_children   (GtkTreeModel      *tree_model,
                                                     GtkTreeIter       *iter,
                                                     GtkTreeIter       *parent);

static gboolean     fake_list_model_iter_has_child  (GtkTreeModel      *tree_model,
                                                     GtkTreeIter       *iter);

static gint         fake_list_model_iter_n_children (GtkTreeModel      *tree_model,
                                                     GtkTreeIter       *iter);

static gboolean     fake_list_model_iter_nth_child  (GtkTreeModel      *tree_model,
                                                     GtkTreeIter       *iter,
                                                     GtkTreeIter       *parent,
                                                     gint               n);

static gboolean     fake_list_model_iter_parent     (GtkTreeModel      *tree_model,
                                                     GtkTreeIter       *iter,
                                                     GtkTreeIter       *child);



static GObjectClass *parent_class = NULL;  /* GObject stuff - nothing to worry about */


/*****************************************************************************
 *
 *  fake_list_model_get_type: here we register our new type and its interfaces
 *                            with the type system. If you want to implement
 *                            additional interfaces like GtkTreeSortable, you
 *                            will need to do it here.
 *
 *****************************************************************************/

GType
fake_list_model_get_type (void)
{
  static GType fake_list_model_type = 0;

  /* Some boilerplate type registration stuff */
  if (fake_list_model_type == 0)
  {
    static const GTypeInfo fake_list_model_info =
    {
      sizeof (FakeListModelClass),
      NULL,                                         /* base_init */
      NULL,                                         /* base_finalize */
      (GClassInitFunc) fake_list_model_class_init,
      NULL,                                         /* class finalize */
      NULL,                                         /* class_data */
      sizeof (FakeListModel),
      0,                                           /* n_preallocs */
      (GInstanceInitFunc) fake_list_model_init
    };
    static const GInterfaceInfo tree_model_info =
    {
      (GInterfaceInitFunc) fake_list_model_tree_model_init,
      NULL,
      NULL
    };

    /* First register the new derived type with the GObject type system */
    fake_list_model_type = g_type_register_static (G_TYPE_OBJECT, "FakeListModel",
                                                   &fake_list_model_info, (GTypeFlags)0);

    /* Now register our GtkTreeModel interface with the type system */
    g_type_add_interface_static (fake_list_model_type, GTK_TYPE_TREE_MODEL, &tree_model_info);
  }

  return fake_list_model_type;
}


/*****************************************************************************
 *
 *  fake_list_model_class_init: more boilerplate GObject/GType stuff.
 *                              Init callback for the type system,
 *                              called once when our new class is created.
 *
 *****************************************************************************/

static void
fake_list_model_class_init (FakeListModelClass *klass)
{
  GObjectClass *object_class;

  parent_class = (GObjectClass*) g_type_class_peek_parent (klass);
  object_class = (GObjectClass*) klass;

  object_class->finalize = fake_list_model_finalize;
}

/*****************************************************************************
 *
 *  fake_list_model_tree_model_init: init callback for the interface registration
 *                                   in custom_list_get_type. Here we override
 *                                   the GtkTreeModel interface functions that
 *                                   we implement.
 *
 *****************************************************************************/

static void
fake_list_model_tree_model_init (GtkTreeModelIface *iface)
{
  iface->get_flags       = fake_list_model_get_flags;
  iface->get_n_columns   = fake_list_model_get_n_columns;
  iface->get_column_type = fake_list_model_get_column_type;
  iface->get_iter        = fake_list_model_get_iter;
  iface->get_path        = fake_list_model_get_path;
  iface->get_value       = fake_list_model_get_value;
  iface->iter_next       = fake_list_model_iter_next;
  iface->iter_children   = fake_list_model_iter_children;
  iface->iter_has_child  = fake_list_model_iter_has_child;
  iface->iter_n_children = fake_list_model_iter_n_children;
  iface->iter_nth_child  = fake_list_model_iter_nth_child;
  iface->iter_parent     = fake_list_model_iter_parent;
}


/*****************************************************************************
 *
 *  fake_list_model_init: this is called everytime a new custom list object
 *                        instance is created (we do that in fake_list_model_new).
 *                        Initialise the list structure's fields here.
 *
 *****************************************************************************/

static void
fake_list_model_init (FakeListModel *fake_list_model)
{
  fake_list_model->stamp = g_random_int();  /* Random int to check whether an iter belongs to our model */
}


/*****************************************************************************
 *
 *  fake_list_model_finalize: this is called just before a custom list is
 *                            destroyed. Free dynamically allocated memory here.
 *
 *****************************************************************************/

static void
fake_list_model_finalize (GObject *object)
{
  /* nothing to do */
  /* but must chain up - finalize parent */
  (* parent_class->finalize) (object);
}


/*****************************************************************************
 *
 *  fake_list_model_get_flags: tells the rest of the world whether our tree model
 *                             has any special characteristics. In our case,
 *                             we have a list model (instead of a tree), and each
 *                             tree iter is valid as long as the row in question
 *                             exists, as it only contains a pointer to our struct.
 *
 *****************************************************************************/

static GtkTreeModelFlags
fake_list_model_get_flags (GtkTreeModel *tree_model)
{
  g_assert(IS_FAKE_LIST_MODEL(tree_model));

  return (GtkTreeModelFlags) GTK_TREE_MODEL_LIST_ONLY;
}


/*****************************************************************************
 *
 *  fake_list_model_get_n_columns: tells the rest of the world how many data
 *                                 columns we export via the tree model interface
 *
 *****************************************************************************/

static gint
fake_list_model_get_n_columns (GtkTreeModel *tree_model)
{
  g_assert(IS_FAKE_LIST_MODEL(tree_model));

  return 1;
}


/*****************************************************************************
 *
 *  fake_list_model_get_column_type: tells the rest of the world which type of
 *                                   data an exported model column contains
 *
 *****************************************************************************/

static GType
fake_list_model_get_column_type (GtkTreeModel *tree_model,
                                 gint          index)
{
  FakeListModel *fake_list_model;

  g_return_val_if_fail (IS_FAKE_LIST_MODEL(tree_model), G_TYPE_INVALID);
  fake_list_model = FAKE_LIST_MODEL(tree_model);
  g_return_val_if_fail (index == 0, G_TYPE_INVALID);

  return G_TYPE_POINTER;
}


/*****************************************************************************
 *
 *  fake_list_model_get_iter: converts a tree path (physical position) into a
 *                            tree iter structure (the content of the iter
 *                            fields will only be used internally by our model).
 *
 *****************************************************************************/

static gboolean
fake_list_model_get_iter (GtkTreeModel *tree_model,
                          GtkTreeIter  *iter,
                          GtkTreePath  *path)
{
  FakeListModel  *fake_list_model;
  gint           *indices
  gint            n;

  g_assert(IS_FAKE_LIST_MODEL(tree_model));
  g_assert(path!=NULL);

  /*fake_list_model = FAKE_LIST_MODEL(tree_model);*/

  indices = gtk_tree_path_get_indices(path);
  depth   = gtk_tree_path_get_depth(path);

  g_assert(depth == 1);
  n = indices[0];
  
  /*g_assert(n < v->size());*/

  /*TODO can we just do it? is it ok with persistent  iters?
  /* and if we use a list, can we store an iterator? */
  iter->stamp      = fake_list_model->stamp;
  iter->user_data  = GINT_TO_POINTER(n);
  iter->user_data2 = NULL;   /* unused */
  iter->user_data3 = NULL;   /* unused */

  return TRUE;
}


/*****************************************************************************
 *
 *  fake_list_model_get_path: converts a tree iter into a tree path (ie. the
 *                            physical position of that row in the list).
 *
 *****************************************************************************/

static GtkTreePath *
fake_list_model_get_path (GtkTreeModel *tree_model,
                          GtkTreeIter  *iter)
{
  FakeListModel *fake_list_model;
  GtkTreePath   *path;
  gint           n;

  g_return_val_if_fail (IS_FAKE_LIST_MODEL(tree_model), NULL);
  /*fake_list_model = FAKE_LIST_MODEL(tree_model);*/

  g_return_val_if_fail (iter != NULL, NULL);

  n = GPOINTER_TO_INT(iter->user_data);
  /*g_return_val_if_fail( n < fake_list_model->v->size(), NULL );*/

  path = gtk_tree_path_new();
  gtk_tree_path_append_index(path, n);

  return path;
}


/*****************************************************************************
 *
 *  fake_list_model_get_value: Returns a row's exported data columns
 *                             (_get_value is what gtk_tree_model_get uses)
 *
 *****************************************************************************/
/*TODO*/
static void
fake_list_model_get_value (GtkTreeModel *tree_model,
                           GtkTreeIter  *iter,
                           gint          column,
                           GValue       *value)
{
  FakeListModel    *fake_list_model;
  int i, j;

  g_return_if_fail (IS_FAKE_LIST_MODEL (tree_model));
  fake_list_model = FAKE_LIST_MODEL(tree_model);

  g_return_if_fail (iter != NULL);
  g_return_if_fail (column < fake_list_model->n_columns);

  i = GPOINTER_TO_INT(iter->user_data);
  j = GPOINTER_TO_INT(iter->user_data2);

  g_return_if_fail ( i >= 0 && i < fake_list_model->p_model->size() );
  g_return_if_fail ( j < 0 || j < fake_list_model->p_model->at(i)->elements.size() );
  
  g_value_init (value, FAKE_LIST_MODEL(tree_model)->column_types[column]);

  switch(column)
  {
    case FAKE_LIST_MODEL_COL_RECORD:
      element_t *e = (j < 0) ?
                     fake_list_model->p_model->at(i).get() :
                     fake_list_model->p_model->at(i)->elements[j].get();
      g_value_set_pointer(value, (gpointer) e);
      break;

    /*case CUSTOM_LIST_COL_NAME:
      g_value_set_string(value, record->name);
      break;

    case CUSTOM_LIST_COL_YEAR_BORN:
      g_value_set_uint(value, record->year_born);
      break;*/
  }
}


/*****************************************************************************
 *
 *  fake_list_model_iter_next: Takes an iter structure and sets it to point
 *                            to the next row (at the same level)
 *
 *****************************************************************************/

static gboolean
fake_list_model_iter_next (GtkTreeModel  *tree_model,
                          GtkTreeIter   *iter)
{
  FakeListModel    *fake_list_model;
  gint i, j;

  g_return_val_if_fail (IS_FAKE_LIST_MODEL (tree_model), FALSE);
  fake_list_model = FAKE_LIST_MODEL(tree_model);

  if (iter == NULL)
    return FALSE;
  
  i = GPOINTER_TO_INT(iter->user_data);
  j = GPOINTER_TO_INT(iter->user_data2);
  
  g_return_val_if_fail ( i >= 0 && i < fake_list_model->p_model->size() , FALSE );
  g_return_val_if_fail ( j < 0 || j < fake_list_model->p_model->at(i)->elements.size() , FALSE );
  
  if (j < 0)
    j = -1;

  /* try first to increment j */
  if (j + 1 < safe<int>(p_model->at(i)->elements.size()))
    j++;
  else {
    /* could not increment j, so we should increment i and set j=-1 */
    /* but are we out of bound? */
    if (i + 1 < safe<int>(p_model->size())) {
      /* no, it's ok */
      i++;
      j = -1;
    }
    else
      return FALSE;
  }
  
  iter->stamp      = fake_list_model->stamp;
  iter->user_data  = GINT_TO_POINTER(i);
  iter->user_data2 = GINT_TO_POINTER(j);
  iter->user_data3 = NULL;   /* unused */

  return TRUE;
}


/*****************************************************************************
 *
 *  fake_list_model_iter_children: Returns TRUE or FALSE depending on whether
 *                                the row specified by 'parent' has any children.
 *                                If it has children, then 'iter' is set to
 *                                point to the first child. Special case: if
 *                                'parent' is NULL, then the first top-level
 *                                row should be returned if it exists.
 *
 *****************************************************************************/

static gboolean
fake_list_model_iter_children (GtkTreeModel *tree_model,
                              GtkTreeIter  *iter,
                              GtkTreeIter  *parent)
{
  FakeListModel  *fake_list_model;
  
  g_return_val_if_fail (IS_FAKE_LIST_MODEL (tree_model), FALSE);
  fake_list_model = FAKE_LIST_MODEL(tree_model);

  if (parent == NULL) {
    if (p_model->size() > 0) {
      iter->stamp      = fake_list_model->stamp;
      iter->user_data  = GINT_TO_POINTER(0);
      iter->user_data2 = GINT_TO_POINTER(-1);
      iter->user_data3 = NULL;   /* unused */

      return TRUE;
    }
  }
  
  return FALSE;
}


/*****************************************************************************
 *
 *  fake_list_model_iter_has_child: Returns TRUE or FALSE depending on whether
 *                              the row specified by 'iter' has any children.
 *
 *****************************************************************************/

static gboolean
fake_list_model_iter_has_child (GtkTreeModel *tree_model,
                               GtkTreeIter  *iter)
{
  g_return_val_if_fail (IS_FAKE_LIST_MODEL (tree_model), FALSE);

  return FALSE;
}


/*****************************************************************************
 *
 *  fake_list_model_iter_n_children: Returns the number of children the row
 *                                  specified by 'iter' has. A special case is
 *                                  when 'iter' is NULL, in which case we need
 *                                  to return the number of top-level nodes.
 *
 *****************************************************************************/

static gint
fake_list_model_iter_n_children (GtkTreeModel *tree_model,
                                GtkTreeIter  *iter)
{
  FakeListModel  *fake_list_model;

  g_return_val_if_fail (IS_FAKE_LIST_MODEL (tree_model), -1);

  fake_list_model = FAKE_LIST_MODEL(tree_model);
  
  if (iter == NULL) {
    int count = 0;
    int A = safe<int>(fake_list_model->p_model->size());

    for (int a = 0; a < A; a++) {
      int B = safe<int>(fake_list_model->p_model->at(a).elements.size());
      count += 1;
      count += B;
    }
    return count;
  }
  
  return 0;
}


/*****************************************************************************
 *
 *  fake_list_model_iter_nth_child: If the row specified by 'parent' has any
 *                                 children, set 'iter' to the n-th child and
 *                                 return TRUE if it exists, otherwise FALSE.
 *                                 A special case is when 'parent' is NULL, in
 *                                 which case we need to set 'iter' to the n-th
 *                                 row if it exists.
 *
 *****************************************************************************/

static gboolean
fake_list_model_iter_nth_child (GtkTreeModel *tree_model,
                               GtkTreeIter  *iter,
                               GtkTreeIter  *parent,
                               gint          n)
{
  FakeListModel    *fake_list_model;
  gint i, j;

  g_return_val_if_fail( IS_FAKE_LIST_MODEL(tree_model), FALSE );
  g_return_val_if_fail( n >= 0, FALSE );

  fake_list_model = FAKE_LIST_MODEL(tree_model);

  if (parent == NULL) {
    if ( n < fake_list_model->p_model->size() ) {
      iter->stamp      = fake_list_model->stamp;
      iter->user_data  = GINT_TO_POINTER(n);
      iter->user_data2 = GINT_TO_POINTER(-1);
      iter->user_data3 = NULL;
    }
    else return FALSE;
  }
  else {
    i = GPOINTER_TO_INT(parent->user_data);
    j = GPOINTER_TO_INT(parent->user_data2);
  
    g_assert ( i >= 0 && i < fake_list_model->p_model->size() );
    g_assert ( j < 0 || j < fake_list_model->p_model->at(i)->elements.size() );
    
    if (j >= 0) return FALSE; /* if current level is 2 we don't have any children */
    
    if ( n < fake_list_model->p_model->at(i)->elements.size() ) {
      iter->stamp      = fake_list_model->stamp;
      iter->user_data  = GINT_TO_POINTER(i);
      iter->user_data2 = GINT_TO_POINTER(n);
      iter->user_data3 = NULL;
    }
    else return FALSE;
  }

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
fake_list_model_iter_parent (GtkTreeModel *tree_model,
                         GtkTreeIter  *iter,
                         GtkTreeIter  *child)
{
  FakeListModel    *fake_list_model;
  gint i, j;

  g_return_val_if_fail( IS_FAKE_LIST_MODEL(tree_model), FALSE );

  fake_list_model = FAKE_LIST_MODEL(tree_model);
  
  i = GPOINTER_TO_INT(iter->user_data);
  j = GPOINTER_TO_INT(iter->user_data2);
  
  g_return_val_if_fail ( i >= 0 && i < fake_list_model->p_model->size() , FALSE );
  g_return_val_if_fail ( j < 0 || j < fake_list_model->p_model->at(i)->elements.size() , FALSE );
  
  if (j < 0) { /* level 1 */
    iter = NULL;
    return FALSE;
  }
  else { /* level 2 */
    j = -1;
    iter->stamp      = fake_list_model->stamp;
    iter->user_data  = GINT_TO_POINTER(i);
    iter->user_data2 = GINT_TO_POINTER(j);
    iter->user_data3 = NULL;
  }
  
  return TRUE;
}

