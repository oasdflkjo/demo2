#include "particle_system.h"
#include "gl_loader.h"
#include "shader_manager.h"
#include "window_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WORK_GROUP_SIZE 256

struct ParticleSystem {
    int max_particles;
    GLuint vao, vbo;
    GLuint compute_shader;
    GLuint render_shader;
    GLuint ssbo;
    WindowManager* window_manager;
};

ParticleSystem* particle_system_create(int max_particles, WindowManager* window_manager) {
    ParticleSystem* ps = (ParticleSystem*)malloc(sizeof(ParticleSystem));
    if (ps) {
        ps->max_particles = max_particles;
        ps->window_manager = window_manager;

        // Create and bind VAO and VBO
        glGenVertexArrays(1, &ps->vao);
        glGenBuffers(1, &ps->vbo);

        glBindVertexArray(ps->vao);
        glBindBuffer(GL_ARRAY_BUFFER, ps->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * max_particles, NULL, GL_DYNAMIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Create SSBO for compute shader
        glGenBuffers(1, &ps->ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ps->ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * max_particles, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Initialize particles
        Particle* particles = (Particle*)malloc(sizeof(Particle) * max_particles);
        for (int i = 0; i < max_particles; i++) {
            particles[i].x = ((float)rand() / RAND_MAX) * 2 - 1;  // Range: -1 to 1
            particles[i].y = ((float)rand() / RAND_MAX) * 2 - 1;  // Range: -1 to 1
            particles[i].vx = ((float)rand() / RAND_MAX) * 0.1f - 0.05f;  // Small initial velocity
            particles[i].vy = ((float)rand() / RAND_MAX) * 0.1f - 0.05f;  // Small initial velocity
        }

        // Print debug information for the first few particles
        printf("First 5 particles:\n");
        for (int i = 0; i < 5 && i < max_particles; i++) {
            printf("Particle %d: pos(%.2f, %.2f), vel(%.2f, %.2f)\n",
                   i, particles[i].x, particles[i].y, particles[i].vx, particles[i].vy);
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ps->ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Particle) * max_particles, particles);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        free(particles);

        // Load compute shader
        ps->compute_shader = create_compute_shader("shaders/particle.comp");
        if (!ps->compute_shader) {
            fprintf(stderr, "Failed to create compute shader for particle system\n");
            free(ps);
            return NULL;
        }
        printf("Compute shader created successfully\n");

        // Load render shader
        ps->render_shader = create_program("shaders/particle.vert", "shaders/particle.frag");
        if (!ps->render_shader) {
            fprintf(stderr, "Failed to create render shader for particle system\n");
            free(ps);
            return NULL;
        }
        printf("Render shader created successfully\n");
    }
    return ps;
}

void particle_system_update(ParticleSystem* ps, float delta_time, float mouse_x, float mouse_y) {
    glUseProgram(ps->compute_shader);
    
    // Set uniforms
    glUniform1f(glGetUniformLocation(ps->compute_shader, "delta_time"), delta_time);
    glUniform2f(glGetUniformLocation(ps->compute_shader, "mouse_pos"), mouse_x, mouse_y);
    
    // Calculate and set aspect ratio
    int width = window_manager_get_width(ps->window_manager);
    int height = window_manager_get_height(ps->window_manager);
    float aspect_ratio = (float)width / height;
    glUniform1f(glGetUniformLocation(ps->compute_shader, "aspect_ratio"), aspect_ratio);
    
    // Print uniform values
    printf("Uniforms: delta_time=%.4f, mouse_pos=(%.2f, %.2f), aspect_ratio=%.2f\n",
           delta_time, mouse_x, mouse_y, aspect_ratio);
    
    // Bind SSBO
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ps->ssbo);
    
    // Dispatch compute shader
    glDispatchCompute(ps->max_particles / WORK_GROUP_SIZE + 1, 1, 1);
    
    // Wait for compute shader to finish
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void particle_system_render(ParticleSystem* ps) {
    glUseProgram(ps->render_shader);
    glBindVertexArray(ps->vao);
    
    // Bind SSBO to the VAO
    glBindBuffer(GL_ARRAY_BUFFER, ps->ssbo);
    
    // Update attribute pointers (only position now)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glEnableVertexAttribArray(0);
    
    glPointSize(2.0f);
    glDrawArrays(GL_POINTS, 0, ps->max_particles);
    
    // Print debug information
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        printf("OpenGL error in particle_system_render: %d\n", error);
    }
    
    // Debug: Print the first few particles' positions
    glBindBuffer(GL_ARRAY_BUFFER, ps->ssbo);
    Particle* particles = (Particle*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    if (particles) {
        printf("First 5 particles after update:\n");
        for (int i = 0; i < 5 && i < ps->max_particles; i++) {
            printf("Particle %d: pos(%.2f, %.2f), vel(%.2f, %.2f)\n",
                   i, particles[i].x, particles[i].y, particles[i].vx, particles[i].vy);
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    } else {
        printf("Failed to map buffer for reading\n");
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void particle_system_destroy(ParticleSystem* ps) {
    if (ps) {
        glDeleteVertexArrays(1, &ps->vao);
        glDeleteBuffers(1, &ps->vbo);
        glDeleteBuffers(1, &ps->ssbo);
        glDeleteProgram(ps->compute_shader);
        glDeleteProgram(ps->render_shader);
        free(ps);
    }
}