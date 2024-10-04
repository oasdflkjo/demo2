#include "particle_system.h"
#include "gl_loader.h"
#include "shader_manager.h"
#include "window_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Define M_PI if it's not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Remove these defines
// #define WINDOW_WIDTH 800
// #define WINDOW_HEIGHT 600

struct ParticleSystem {
    Particle* particles;
    int max_particles;
    int active_particles;
    GLuint vao, vbo;
    GLuint shader_program;
    WindowManager* window_manager;  // Add this line
};

ParticleSystem* particle_system_create(int max_particles, WindowManager* window_manager) {
    ParticleSystem* ps = (ParticleSystem*)malloc(sizeof(ParticleSystem));
    if (ps) {
        ps->particles = (Particle*)malloc(sizeof(Particle) * max_particles);
        ps->max_particles = max_particles;
        ps->active_particles = max_particles;
        ps->window_manager = window_manager;  // Add this line
        
        int width = window_manager_get_width(window_manager);
        int height = window_manager_get_height(window_manager);
        float aspect_ratio = (float)width / height;

        for (int i = 0; i < max_particles; i++) {
            float angle = ((float)rand() / RAND_MAX) * 2 * M_PI;
            float radius = sqrt((float)rand() / RAND_MAX) * 0.5f;
            
            ps->particles[i].x = cos(angle) * radius / aspect_ratio;
            ps->particles[i].y = sin(angle) * radius;
            
            float speed = ((float)rand() / RAND_MAX) * 0.02f;
            ps->particles[i].vx = cos(angle) * speed / aspect_ratio;
            ps->particles[i].vy = sin(angle) * speed;
            
            ps->particles[i].r = (float)rand() / RAND_MAX;
            ps->particles[i].g = (float)rand() / RAND_MAX;
            ps->particles[i].b = (float)rand() / RAND_MAX;
            ps->particles[i].lifetime = (float)rand() / RAND_MAX * 5.0f;  // 0-5 seconds lifetime
        }

        // Create and bind VAO and VBO
        glGenVertexArrays(1, &ps->vao);
        glGenBuffers(1, &ps->vbo);

        glBindVertexArray(ps->vao);
        glBindBuffer(GL_ARRAY_BUFFER, ps->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * max_particles, ps->particles, GL_DYNAMIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Load shader program
        ps->shader_program = create_program("shaders/particle.vert", "shaders/particle.frag");
        if (!ps->shader_program) {
            fprintf(stderr, "Failed to create shader program for particle system\n");
            free(ps->particles);
            free(ps);
            return NULL;
        }
    }
    return ps;
}

void particle_system_update(ParticleSystem* ps, float delta_time, int mouse_x, int mouse_y) {
    int width = window_manager_get_width(ps->window_manager);
    int height = window_manager_get_height(ps->window_manager);
    float aspect_ratio = (float)width / height;

    // Convert mouse coordinates to OpenGL coordinate system
    float mouse_gl_x = (float)mouse_x / (width / 2) - 1.0f;
    float mouse_gl_y = -((float)mouse_y / (height / 2) - 1.0f);  // Invert Y-axis

    for (int i = 0; i < ps->max_particles; i++) {
        Particle* p = &ps->particles[i];
        
        p->x += p->vx * delta_time;
        p->y += p->vy * delta_time;
        p->lifetime -= delta_time;

        // Calculate distance from center, taking aspect ratio into account
        float dist = sqrt((p->x * aspect_ratio) * (p->x * aspect_ratio) + p->y * p->y);

        // Reset particle if it's dead or too far from center
        if (p->lifetime <= 0 || dist > 1.0f) {
            float angle = ((float)rand() / RAND_MAX) * 2 * M_PI;
            float radius = sqrt((float)rand() / RAND_MAX) * 0.1f; // Small initial radius
            
            p->x = mouse_gl_x + cos(angle) * radius / aspect_ratio;
            p->y = mouse_gl_y + sin(angle) * radius;
            
            float speed = ((float)rand() / RAND_MAX) * 0.02f;
            p->vx = cos(angle) * speed / aspect_ratio;
            p->vy = sin(angle) * speed;
            
            p->lifetime = (float)rand() / RAND_MAX * 5.0f;
        }
    }

    // Update VBO data
    glBindBuffer(GL_ARRAY_BUFFER, ps->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle) * ps->max_particles, ps->particles);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void particle_system_render(ParticleSystem* ps) {
    glUseProgram(ps->shader_program);
    glBindVertexArray(ps->vao);
    glDrawArrays(GL_POINTS, 0, ps->max_particles);
    glBindVertexArray(0);
    glUseProgram(0);
}

void particle_system_destroy(ParticleSystem* ps) {
    if (ps) {
        glDeleteVertexArrays(1, &ps->vao);
        glDeleteBuffers(1, &ps->vbo);
        glDeleteProgram(ps->shader_program);
        free(ps->particles);
        free(ps);
    }
}