#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

typedef struct Engine Engine;

Engine* engine_create(void);
int engine_initialize(Engine* engine);  // Changed from bool to int
void engine_run(Engine* engine);
void engine_shutdown(Engine* engine);
void engine_destroy(Engine* engine);

#endif // ENGINE_H
