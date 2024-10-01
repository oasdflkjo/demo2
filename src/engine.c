#include "engine.h"
#include "window_manager.h"
#include "input_manager.h"
#include "renderer.h"
#include "particle_system.h"
#include "shader_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <gl/gl.h>
#include <Windows.h>  // For GetTickCount64()

struct Engine {
    WindowManager* window_manager;
    InputManager* input_manager;
    Renderer* renderer;
    ParticleSystem* particle_system;
    ShaderManager* shader_manager;
    int is_running;
    float delta_time;
    float last_frame_time;
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
        engine->delta_time = 0.0f;
        engine->last_frame_time = 0.0f;
    }
    return engine;
}

bool engine_initialize(Engine* engine) {
    printf("Initializing engine...\n");
    
    engine->window_manager = window_manager_create();
    if (!engine->window_manager || !window_manager_initialize(engine->window_manager)) {
        fprintf(stderr, "Failed to initialize window manager\n");
        return false;
    }

    // TODO: Initialize other subsystems
    engine->input_manager = NULL;
    engine->renderer = NULL;
    engine->shader_manager = NULL;

    // Create particle system
    engine->particle_system = particle_system_create(1000);
    if (!engine->particle_system) {
        fprintf(stderr, "Failed to create particle system\n");
        return false;
    }

    engine->is_running = 1;
    printf("Engine initialized successfully.\n");
    return true;
}

bool engine_is_running(Engine* engine) {
    return engine->is_running == 1;
}

bool engine_update(Engine* engine) {
    // Calculate delta time
    ULONGLONG current_time = GetTickCount64();
    float current_time_seconds = current_time / 1000.0f;
    
    engine->delta_time = current_time_seconds - engine->last_frame_time;
    engine->last_frame_time = current_time_seconds;

    // Update window and check if it should close
    if (!window_manager_update(engine->window_manager)) {
        engine->is_running = 0;  // Set is_running to 0 to exit the main loop
        return false;
    }

    // TODO: Implement actual update logic
    // input_manager_update(engine->input_manager);

    // Update particle system
    particle_system_update(engine->particle_system, engine->delta_time);

    return true;
}

void engine_render(Engine* engine) {
    // Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render particle system
    particle_system_render(engine->particle_system);

    // Swap buffers
    SwapBuffers(window_manager_get_dc(engine->window_manager));
}

void engine_shutdown(Engine* engine) {
    printf("Shutting down engine...\n");
    if (engine->window_manager) {
        window_manager_shutdown(engine->window_manager);
    }
    // TODO: Shutdown other subsystems
    
    if (engine->particle_system) {
        particle_system_destroy(engine->particle_system);
        engine->particle_system = NULL;
    }
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
