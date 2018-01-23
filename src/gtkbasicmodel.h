#ifndef REMUX_GTK_MODEL_H
#define REMUX_GTK_MODEL_H

#include <gtk/gtk.h>

/* Some boilerplate GObject defines. 'klass' is used
 * instead of 'class', because 'class' is a C++ keyword */

#define TYPE_BASIC_LIST_MODEL            (basic_list_model_get_type ())

#define BASIC_LIST_MODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_BASIC_LIST_MODEL, BasicListModel))
#define IS_BASIC_LIST_MODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_BASIC_LIST_MODEL))

#define BASIC_LIST_MODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  TYPE_BASIC_LIST_MODEL, BasicListModelClass))
#define IS_BASIC_LIST_MODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  TYPE_BASIC_LIST_MODEL))

#define BASIC_LIST_MODEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  TYPE_BASIC_LIST_MODEL, BasicListModelClass))

typedef struct _BasicListModel {
	GObject         parent;      /* this MUST be the first member */
	
	gint            numrows;
	
	gint            stamp;       /* Random integer to check whether an iter belongs to our model */
} BasicListModel;

/* CustomListClass: more boilerplate GObject stuff */

typedef struct _BasicListModelClass
{
  GObjectClass parent_class;
} BasicListModelClass;


GType             basic_list_model_get_type (void);

BasicListModel   *basic_list_model_new (gint numrows);

#endif
