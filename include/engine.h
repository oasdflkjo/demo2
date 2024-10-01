#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

typedef struct Engine Engine;

Engine* engine_create(void);
bool engine_initialize(Engine* engine);
bool engine_update(Engine* engine);
void engine_render(Engine* engine);
bool engine_is_running(Engine* engine);
void engine_shutdown(Engine* engine);
void engine_destroy(Engine* engine);

#endif // ENGINE_H
