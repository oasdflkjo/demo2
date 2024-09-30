#ifndef ENGINE_H
#define ENGINE_H

typedef struct Engine Engine;

Engine* engine_create(void);
int engine_initialize(Engine* engine);
int engine_is_running(Engine* engine);
void engine_update(Engine* engine);
void engine_render(Engine* engine);
void engine_shutdown(Engine* engine);
void engine_destroy(Engine* engine);

#endif // ENGINE_H
