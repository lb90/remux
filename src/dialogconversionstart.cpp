#include <cstdlib>
#include <cassert>
#include <ctime>
#include <gtk/gtk.h>
#include "app.h"
#include "window.h"
#include "dialogconversion.h"
#include "dialogconversionstart.h"

dialogconversionstart::dialogconversionstart(GtkWindow *window)
{
	builder          = gtk_builder_new_from_resource("/org/remux/remux/ui/conversionstart.ui");
	dialog           = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));

	check_use_timer  = GTK_WIDGET(gtk_builder_get_object(builder, "check_use_timer"));
	label_h          = GTK_WIDGET(gtk_builder_get_object(builder, "label_h"));
	spinbutton_h     = GTK_WIDGET(gtk_builder_get_object(builder, "spinbutton_h"));
	label_m          = GTK_WIDGET(gtk_builder_get_object(builder, "label_m"));
	spinbutton_m     = GTK_WIDGET(gtk_builder_get_object(builder, "spinbutton_m"));
	
	button_ok        = GTK_WIDGET(gtk_builder_get_object(builder, "button_ok"));
	button_close     = GTK_WIDGET(gtk_builder_get_object(builder, "button_close"));
	
	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
	
	g_signal_connect(dialog, "destroy", G_CALLBACK(self_deleter), (gpointer) this);
	
	g_signal_connect(check_use_timer, "toggled", G_CALLBACK(cb_toggled_use_timer), (gpointer) this);
	g_signal_connect(button_ok, "clicked", G_CALLBACK(cb_ok), (gpointer) this);
	g_signal_connect(button_close, "clicked", G_CALLBACK(cb_close), (gpointer) this);
	
	if (app::has_timer) {
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_use_timer), TRUE);
	    
	    GtkAdjustment *adj_h = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(spinbutton_h));
        GtkAdjustment *adj_m = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(spinbutton_m));
        
        gtk_adjustment_set_value(adj_h, double(app::hour));
        gtk_adjustment_set_value(adj_m, double(app::minute));
	}
}

void dialogconversionstart::show() {
	gtk_window_present(GTK_WINDOW(dialog));
}

void dialogconversionstart::cb_toggled_use_timer(GtkToggleButton *toggle, void *self) {
    dialogconversionstart *inst;
    
    inst = (dialogconversionstart*) self;

    bool active = gtk_toggle_button_get_active(toggle);
    
    gtk_widget_set_sensitive(inst->label_h, active);
    gtk_widget_set_sensitive(inst->spinbutton_h, active);
    gtk_widget_set_sensitive(inst->label_m, active);
    gtk_widget_set_sensitive(inst->spinbutton_m, active);
    
    GtkAdjustment *adj_h = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(inst->spinbutton_h));
    GtkAdjustment *adj_m = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(inst->spinbutton_m));
    if (active) {
        time_t t = time(NULL);
        struct tm *lt = localtime(&t);

        gtk_adjustment_set_value(adj_h, double(lt->tm_hour));
        gtk_adjustment_set_value(adj_m, double(lt->tm_min));
    }
    else {
        gtk_adjustment_set_value(adj_h, 0.0f);
        gtk_adjustment_set_value(adj_m, 0.0f);
    }
}

void dialogconversionstart::cb_ok(GtkButton *button, void *self) {
    dialogconversionstart *inst;
    
    inst = (dialogconversionstart*) self;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(inst->check_use_timer))) {
	    int h, m;
	    
	    GtkAdjustment *adj_h = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(inst->spinbutton_h));
        GtkAdjustment *adj_m = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(inst->spinbutton_m));
        
        h = (int) gtk_adjustment_get_value(adj_h);
        m = (int) gtk_adjustment_get_value(adj_m);
        
	    app::set_conversion_timer(h, m);
	    
	    gtk_widget_destroy(GTK_WIDGET(inst->dialog));
	}
	else {
	    app::remove_conversion_timer();
	
	    gtk_widget_destroy(GTK_WIDGET(inst->dialog)); /*TODO not very safe */

	    dialogconversion *dialog = new dialogconversion(get_window(NULL));
        dialog->show();
    }
}

void dialogconversionstart::cb_close(GtkButton *button, void *self) {
    dialogconversionstart *inst;
    
    inst = (dialogconversionstart*) self;

    gtk_widget_destroy(GTK_WIDGET(inst->dialog));
}

dialogconversionstart::~dialogconversionstart() {
	g_object_unref(builder);
}

void dialogconversionstart::self_deleter(GtkDialog *dialog, gpointer self) {
	dialogconversionstart *inst = (dialogconversionstart*) self;
	assert(dialog == inst->dialog);
	delete inst;
}

