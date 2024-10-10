#include "engine.h"
#include "window_manager.h"
#include "renderer.h"
#include "input_manager.h"
#include "particle_system.h"
#include "gl_loader.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

struct Engine
{
    WindowManager *window_manager;
    InputManager *input_manager;
    ParticleSystem *particle_system;
    bool should_quit;
};

Engine *engine_create(void)
{
    Engine *engine = malloc(sizeof(Engine));
    if (engine)
    {
        engine->window_manager = window_manager_create();
        engine->input_manager = NULL;
        engine->particle_system = NULL;
    }
    return engine;
}

int engine_initialize(Engine *engine)
{
    if (!window_manager_initialize(engine->window_manager))
    {
        return 0;
    }

    HWND window = window_manager_get_window(engine->window_manager);
    engine->input_manager = input_manager_create(window);
    if (!engine->input_manager)
    {
        return 0;
    }

    int width = window_manager_get_width(engine->window_manager);
    int height = window_manager_get_height(engine->window_manager);

    if (!renderer_initialize(width, height))
    {
        // Handle initialization failure
        return 0;
    }

    // Initialize particle system
    srand(time(NULL));                                                       // Seed random number generator
    engine->particle_system = particle_system_create(20000000, width, height); // Create 100,000 particles
    if (!engine->particle_system)
    {
        fprintf(stderr, "Failed to create particle system\n");
        return 0;
    }
    printf("Particle system created successfully\n");

    return 1;
}

void engine_run(Engine *engine)
{
    float last_time = 0;
    int width = window_manager_get_width(engine->window_manager);
    int height = window_manager_get_height(engine->window_manager);
    while (!engine->should_quit && !window_manager_should_close(engine->window_manager))
    {
        float current_time = (float)clock() / CLOCKS_PER_SEC;
        float delta_time = current_time - last_time;
        last_time = current_time;

        input_manager_update(engine->input_manager);

        if (input_manager_is_key_pressed(engine->input_manager, VK_ESCAPE))
        {
            engine->should_quit = true;
        }

        MousePosition mouse_pos = input_manager_get_mouse_position(engine->input_manager);

        // Convert mouse coordinates to OpenGL coordinate system

        float mouse_gl_x = (float)mouse_pos.x / width * 2 - 1;
        float mouse_gl_y = 1 - (float)mouse_pos.y / height * 2;

        // Start rendering to the framebuffer
        renderer_start_frame();

        // Clear the screen with a dark color
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        particle_system_update(engine->particle_system, delta_time, mouse_gl_x, mouse_gl_y);

        renderer_render_particles(engine->particle_system);

        renderer_render_cursor(mouse_gl_x, mouse_gl_y);

        // End rendering to the framebuffer and apply glitch effect
        renderer_end_frame();

        // Swap buffers
        window_manager_swap_buffers(engine->window_manager);
    }
}

void engine_shutdown(Engine *engine)
{
    renderer_shutdown();
    particle_system_destroy(engine->particle_system);
    input_manager_destroy(engine->input_manager);
    window_manager_shutdown(engine->window_manager);
}

void engine_destroy(Engine *engine)
{
    if (engine)
    {
        window_manager_destroy(engine->window_manager);
        free(engine);
    }
}
