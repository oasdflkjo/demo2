#include "engine.h"
#include "window_manager.h"
#include "renderer.h"
#include "input_manager.h"
#include "particle_system.h"
#include <stdlib.h>

struct Engine {
    WindowManager* window_manager;
};

Engine* engine_create(void) {
    Engine* engine = malloc(sizeof(Engine));
    if (engine) {
        engine->window_manager = window_manager_create();
    }
    return engine;
}

int engine_initialize(Engine* engine) {
    if (!window_manager_initialize(engine->window_manager)) {
        return 0;
    }

    if (!renderer_initialize()) {
        return 0;
    }

    // Initialize other systems as needed
    // input_manager_initialize();
    // particle_system_initialize();

    return 1;
}

void engine_run(Engine* engine) {
    while (!window_manager_should_close(engine->window_manager)) {
        // Update game logic
        // input_manager_update();
        // particle_system_update();

        // Render frame
        renderer_render();

        // Swap buffers
        SwapBuffers(window_manager_get_dc(engine->window_manager));
    }
}

void engine_shutdown(Engine* engine) {
    renderer_shutdown();
    // Shutdown other systems
    // input_manager_shutdown();
    // particle_system_shutdown();
    window_manager_shutdown(engine->window_manager);
}

void engine_destroy(Engine* engine) {
    if (engine) {
        window_manager_destroy(engine->window_manager);
        free(engine);
    }
}
