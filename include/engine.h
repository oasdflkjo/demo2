#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>
#include "gl_loader.h"

typedef struct Engine Engine;

Engine* engine_create(void);
int engine_initialize(Engine* engine);
void engine_run(Engine* engine);
void engine_shutdown(Engine* engine);
void engine_destroy(Engine* engine);

#endif // ENGINE_H
