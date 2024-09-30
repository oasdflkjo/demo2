#include "engine.h"
#include "window_manager.h"
#include "input_manager.h"
#include "renderer.h"
#include "particle_system.h"
#include "shader_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <gl/gl.h>

struct Engine {
    WindowManager* window_manager;
    InputManager* input_manager;
    Renderer* renderer;
    ParticleSystem* particle_system;
    ShaderManager* shader_manager;
    int is_running;
};

Engine* engine_create(void) {
    Engine* engine = malloc(sizeof(Engine));
    if (engine) {
        engine->window_manager = NULL;
        engine->input_manager = NULL;
        engine->renderer = NULL;
        engine->particle_system = NULL;
        engine->shader_manager = NULL;
        engine->is_running = 0;
    }
    return engine;
}

int engine_initialize(Engine* engine) {
    printf("Initializing engine...\n");
    
    engine->window_manager = window_manager_create();
    if (!engine->window_manager || !window_manager_initialize(engine->window_manager)) {
        fprintf(stderr, "Failed to initialize window manager\n");
        return 0;
    }

    // TODO: Initialize other subsystems
    engine->input_manager = NULL;
    engine->renderer = NULL;
    engine->particle_system = NULL;
    engine->shader_manager = NULL;

    engine->is_running = 1;
    printf("Engine initialized successfully.\n");
    return 1;
}

int engine_is_running(Engine* engine) {
    return engine->is_running;
}

void engine_update(Engine* engine) {
    if (window_manager_should_close(engine->window_manager)) {
        engine->is_running = 0;
        return;
    }

    // TODO: Implement actual update logic
    // input_manager_update(engine->input_manager);
    // particle_system_update(engine->particle_system);

    window_manager_update(engine->window_manager);
}

void engine_render(Engine* engine) {
    (void)engine;  // Silence unused parameter warning
    printf("Rendering frame...\n");
    
    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: Implement actual rendering logic
}

void engine_shutdown(Engine* engine) {
    printf("Shutting down engine...\n");
    if (engine->window_manager) {
        window_manager_shutdown(engine->window_manager);
    }
    // TODO: Shutdown other subsystems
}

void engine_destroy(Engine* engine) {
    if (engine) {
        printf("Destroying engine...\n");
        if (engine->window_manager) {
            window_manager_destroy(engine->window_manager);
        }
        // TODO: Destroy other subsystems
        free(engine);
        printf("Engine destroyed.\n");
    }
}
