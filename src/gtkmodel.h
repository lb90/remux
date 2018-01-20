#ifndef REMUX_GTK_MODEL_H
#define REMUX_GTK_MODEL_H

#include <gtk/gtk.h>

/* Some boilerplate GObject defines. 'klass' is used
 * instead of 'class', because 'class' is a C++ keyword */

#define TYPE_FAKE_LIST_MODEL            (fake_list_model_get_type ())

#define FAKE_LIST_MODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_FAKE_LIST_MODEL, FakeListModel))
#define IS_FAKE_LIST_MODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_FAKE_LIST_MODEL))

#define FAKE_LIST_MODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  TYPE_FAKE_LIST_MODEL, FakeListModelClass))
#define IS_FAKE_LIST_MODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  TYPE_FAKE_LIST_MODEL))

#define FAKE_LIST_MODEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  TYPE_FAKE_LIST_MODEL, FakeListModelClass))

typedef struct _FakeListModel {
	GObject         parent;      /* this MUST be the first member */
	
	//pointer to vector
	
	gint            stamp;       /* Random integer to check whether an iter belongs to our model */
} FakeListModel;

/* CustomListClass: more boilerplate GObject stuff */

typedef struct _FakeListModelClass
{
  GObjectClass parent_class;
} FakeListModelClass;


GType             fake_list_model_get_type (void);

#endif
