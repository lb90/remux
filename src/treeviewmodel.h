#ifndef STK_TREEVIEWMODEL_H
#define STK_TREEVIEWMODEL_H

#include <cstdlib>
#include <list>
#include <gtk/gtk.h>

enum modelattributes {
	modelattribute_obs = 0,
	N_ATTRIBUTES
};

/* Some boilerplate GObject defines. 'klass' is used
 *   instead of 'class', because 'class' is a C++ keyword */

#define CUSTOM_TYPE_MODEL            (custom_model_get_type ())
#define CUSTOM_MODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUSTOM_TYPE_MODEL, CustomModel))
#define CUSTOM_MODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  CUSTOM_TYPE_MODEL, CustomModelClass))
#define CUSTOM_IS_MODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUSTOM_TYPE_MODEL))
#define CUSTOM_IS_MODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  CUSTOM_TYPE_MODEL))
#define CUSTOM_MODEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  CUSTOM_TYPE_MODEL, CustomModelClass))

//typedef struct _CustomModel       CustomModel;
//typedef struct _CustomModelClass  CustomModelClass;

/* CustomList: this structure contains everything we need for our
 *             model implementation. You can add extra fields to
 *             this structure, e.g. hashtables to quickly lookup
 *             rows or whatever else you might need, but it is
 *             crucial that 'parent' is the first member of the
 *             structure.                                          */

struct CustomModel
{
	GObject         parent;      /* this MUST be the first member */
	gint            stamp;       /* Random integer to check whether an iter belongs to our model */
};



/* CustomListClass: more boilerplate GObject stuff */

struct CustomModelClass
{
	GObjectClass    parentclass;
};


GType             custom_model_get_type(void);
CustomModel       *custom_model_new(void);
void              custom_model_record_appended(CustomModel *custommodel);
void              custom_model_record_changed(CustomModel *custommodel, int n);
void              custom_model_record_removed(CustomModel *custommodel, int n);

#endif
