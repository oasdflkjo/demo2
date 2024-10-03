#include "engine.h"
#include "window_manager.h"
#include "renderer.h"
#include "input_manager.h"
#include "particle_system.h"
#include "gl_loader.h"  // Add this line to include OpenGL function declarations
#include <stdlib.h>
#include <time.h>

struct Engine {
    WindowManager* window_manager;
    ParticleSystem* particle_system;
};

Engine* engine_create(void) {
    Engine* engine = malloc(sizeof(Engine));
    if (engine) {
        engine->window_manager = window_manager_create();
        engine->particle_system = NULL;
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

    // Initialize particle system
    srand(time(NULL));  // Seed random number generator
    engine->particle_system = particle_system_create(10000);  // Create 10,000 particles
    if (!engine->particle_system) {
        return 0;
    }

    return 1;
}

void engine_run(Engine* engine) {
    float last_time = 0;
    while (!window_manager_should_close(engine->window_manager)) {
        float current_time = (float)clock() / CLOCKS_PER_SEC;
        float delta_time = current_time - last_time;
        last_time = current_time;

        // Update particle system
        particle_system_update(engine->particle_system, delta_time);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Render particle system
        particle_system_render(engine->particle_system);

        // Swap buffers
        window_manager_swap_buffers(engine->window_manager);
    }
}

void engine_shutdown(Engine* engine) {
    renderer_shutdown();
    particle_system_destroy(engine->particle_system);
    window_manager_shutdown(engine->window_manager);
}

void engine_destroy(Engine* engine) {
    if (engine) {
        window_manager_destroy(engine->window_manager);
        free(engine);
    }
}
