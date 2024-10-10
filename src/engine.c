#include "engine.h"
#include "window_manager.h"
#include "renderer.h"
#include "input_manager.h"
#include "particle_system.h"
#include "gl_loader.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define TARGET_FPS 160
#define FRAME_TIME (1000.0 / TARGET_FPS)

struct Engine
{
    WindowManager *window_manager;
    InputManager *input_manager;
    ParticleSystem *particle_system;
    bool should_quit;
    float mouse_x;
    float mouse_y;
    HDC hdc;
};

Engine *engine_create(void)
{
    Engine *engine = malloc(sizeof(Engine));
    if (engine)
    {
        engine->window_manager = wm_create();
        engine->input_manager = NULL;
        engine->particle_system = NULL;
        engine->mouse_x = 0.0f;
        engine->mouse_y = 0.0f;
        engine->hdc = NULL;
    }
    return engine;
}

int engine_initialize(Engine *engine)
{
    if (!wm_init(engine->window_manager))
    {
        return 0;
    }

    HWND window = wm_get_window(engine->window_manager);
    engine->input_manager = input_manager_create(window);
    if (!engine->input_manager)
    {
        return 0;
    }

    int width = wm_get_width(engine->window_manager);
    int height = wm_get_height(engine->window_manager);

    if (!renderer_initialize(width, height))
    {
        // Handle initialization failure
        return 0;
    }

    // Initialize particle system
    srand(time(NULL));                                                       // Seed random number generator
    engine->particle_system = particle_system_create(10000000, width, height); // Create 10,000,000 particles
    if (!engine->particle_system)
    {
        fprintf(stderr, "Failed to create particle system\n");
        return 0;
    }
    printf("Particle system created successfully\n");

    // Get the device context
    engine->hdc = GetDC(window);
    if (!engine->hdc)
    {
        fprintf(stderr, "Failed to get device context\n");
        return 0;
    }

    return 1;
}

void engine_run(Engine *engine)
{
    LARGE_INTEGER frequency, start, end, target_time;
    double elapsed_ms, sleep_ms, idle_ms;
    QueryPerformanceFrequency(&frequency);

    // Set timer resolution to 1ms
    timeBeginPeriod(1);

    // Calculate how many QPC ticks equal one frame duration
    LONGLONG ticks_per_frame = (LONGLONG)(frequency.QuadPart / TARGET_FPS);

    LONGLONG oversleep_time = 0;

    while (!engine->should_quit)
    {
        QueryPerformanceCounter(&start);

        // Adjust start time based on previous oversleep
        start.QuadPart -= oversleep_time;

        // Set the target time for this frame
        target_time.QuadPart = start.QuadPart + ticks_per_frame;

        // Process events
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                engine->should_quit = true;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        // Update input
        input_manager_update(engine->input_manager);

        // Check for ESC key to quit
        if (input_manager_is_key_pressed(engine->input_manager, VK_ESCAPE))
        {
            engine->should_quit = true;
            continue;
        }

        // Update mouse position
        POINT mouse_pos;
        GetCursorPos(&mouse_pos);
        ScreenToClient(wm_get_window(engine->window_manager), &mouse_pos);
        engine->mouse_x = (float)mouse_pos.x / wm_get_width(engine->window_manager) * 2 - 1;
        engine->mouse_y = -((float)mouse_pos.y / wm_get_height(engine->window_manager) * 2 - 1);

        // Update game state
        float delta_time = 1.0f / TARGET_FPS;  // Fixed delta time for consistent updates
        particle_system_update(engine->particle_system, delta_time, engine->mouse_x, engine->mouse_y);

        // Render frame
        renderer_start_frame();
        glClear(GL_COLOR_BUFFER_BIT);

        renderer_render_particles(engine->particle_system);
        renderer_render_cursor(engine->mouse_x, engine->mouse_y);

        renderer_end_frame();
        SwapBuffers(engine->hdc);

        // Calculate elapsed time and sleep if necessary
        QueryPerformanceCounter(&end);
        elapsed_ms = (end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;

        // Calculate how long to wait
        LONGLONG wait_time = target_time.QuadPart - end.QuadPart;

        if (wait_time > 0)
        {
            // Convert wait time to milliseconds
            sleep_ms = wait_time * 1000.0 / frequency.QuadPart;

            // Sleep for most of the wait time
            if (sleep_ms > 1.0)
            {
                Sleep((DWORD)(sleep_ms - 1.0));
            }

            // Spin-wait for the remainder
            do {
                QueryPerformanceCounter(&end);
            } while (end.QuadPart < target_time.QuadPart);
        }

        // Calculate actual frame time and idle time
        QueryPerformanceCounter(&end);
        double frame_time = (end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
        idle_ms = frame_time - elapsed_ms;

        // Calculate oversleep for next frame adjustment
        oversleep_time = end.QuadPart - target_time.QuadPart;
        if (oversleep_time < 0) oversleep_time = 0;

        printf("Frame time: %.2f ms, Processing time: %.2f ms, Idle time: %.2f ms\n", 
               frame_time, elapsed_ms, idle_ms);
    }

    // Reset timer resolution
    timeEndPeriod(1);
}

void engine_shutdown(Engine *engine)
{
    renderer_shutdown();
    particle_system_destroy(engine->particle_system);
    input_manager_destroy(engine->input_manager);
    wm_shutdown(engine->window_manager);
    if (engine->hdc)
    {
        ReleaseDC(wm_get_window(engine->window_manager), engine->hdc);
    }
}

void engine_destroy(Engine *engine)
{
    if (engine)
    {
        wm_destroy(engine->window_manager);
        free(engine);
    }
}
