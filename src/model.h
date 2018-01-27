#ifndef REMUX_MODEL_H
#define REMUX_MODEL_H

#include <vector>
#include "elements.h"

int model_clear();
int model_add(const char *name,
              const char *directory);
void model_remove(size_t i);

extern std::vector<media_t> elementv;

#endif

