#include <cstdlib>
#include "signalcentre.h"
#include "model.h"
#include "scandirectory.h"
#include "dialogproperty.h"
#include "window.h"
#include "app.h"

void app::scandirectory(const char *directoryname) {
	model_clear();
	scan_directory(directoryname);
}

void app::elementactivated(int n) {
	assert(n >= 0);
	assert(size_t(n) < elementv.size());
	dialogproperty_t *dialog = new dialogproperty_t(get_window(NULL));
	
	dialog->setcurrentelement(n);
	dialog->show();
}

int app::init() {
	signalcentre::addhandler("treeview\\rowactivated", app::elementactivated);

	return 0;
}

