#include "particle_system.h"
#include "gl_loader.h"
#include "shader_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WORK_GROUP_SIZE 256

struct ParticleSystem {
    int max_particles;
    GLuint ssbo;
    GLuint compute_shader;
    int width;
    int height;
};

ParticleSystem* particle_system_create(int max_particles, int width, int height) {
    ParticleSystem* ps = (ParticleSystem*)malloc(sizeof(ParticleSystem));
    if (ps) {
        ps->max_particles = max_particles;
        ps->width = width;
        ps->height = height;

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
    }
    return ps;
}

void particle_system_update(ParticleSystem* ps, float delta_time, float mouse_x, float mouse_y) {
    glUseProgram(ps->compute_shader);
    
    // Set uniforms
    glUniform1f(glGetUniformLocation(ps->compute_shader, "delta_time"), delta_time);
    glUniform2f(glGetUniformLocation(ps->compute_shader, "mouse_pos"), mouse_x, mouse_y);
    
    // Calculate and set aspect ratio
    float aspect_ratio = (float)ps->width / ps->height;
    glUniform1f(glGetUniformLocation(ps->compute_shader, "aspect_ratio"), aspect_ratio);
    
    // Bind SSBO
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ps->ssbo);
    
    // Dispatch compute shader
    glDispatchCompute(ps->max_particles / WORK_GROUP_SIZE + 1, 1, 1);
    
    // Wait for compute shader to finish
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

GLuint particle_system_get_ssbo(ParticleSystem* ps) {
    return ps->ssbo;
}

int particle_system_get_max_particles(ParticleSystem* ps) {
    return ps->max_particles;
}

void particle_system_destroy(ParticleSystem* ps) {
    if (ps) {
        glDeleteBuffers(1, &ps->ssbo);
        glDeleteProgram(ps->compute_shader);
        free(ps);
    }
}