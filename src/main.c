#include "engine.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    (void)argc;  // Silence unused parameter warning
    (void)argv;  // Silence unused parameter warning

    Engine* engine = engine_create();
    if (!engine) {
        fprintf(stderr, "Failed to create engine\n");
        return 1;
    }

    if (!engine_initialize(engine)) {
        fprintf(stderr, "Failed to initialize engine\n");
        engine_destroy(engine);
        return 1;
    }

    while (engine_is_running(engine)) {
        engine_update(engine);
        engine_render(engine);
    }

    engine_shutdown(engine);
    engine_destroy(engine);

    return 0;
}
