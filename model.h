#ifndef REMUX_MODEL_H
#define REMUX_MODEL_H

int model_init();
int model_clear();
int model_add(bool st,
              const char *name,
              const char *path);

#endif

