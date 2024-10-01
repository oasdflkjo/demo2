#include "engine.h"
#include <stdio.h>

int main(void) {
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

    engine_run(engine);

    engine_shutdown(engine);
    engine_destroy(engine);

    return 0;
}
