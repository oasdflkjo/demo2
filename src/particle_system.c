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
    GLuint position_ssbo;
    GLuint velocity_ssbo;
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

        // Create SSBOs for compute shader
        glGenBuffers(1, &ps->position_ssbo);
        glGenBuffers(1, &ps->velocity_ssbo);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ps->position_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 2 * max_particles, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ps->velocity_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 2 * max_particles, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Initialize particles
        float* position_data = (float*)malloc(sizeof(float) * 2 * max_particles);
        float* velocity_data = (float*)malloc(sizeof(float) * 2 * max_particles);
        for (int i = 0; i < max_particles; i++) {
            position_data[i * 2 + 0] = ((float)rand() / RAND_MAX) * 2 - 1;  // x position
            position_data[i * 2 + 1] = ((float)rand() / RAND_MAX) * 2 - 1;  // y position
            velocity_data[i * 2 + 0] = ((float)rand() / RAND_MAX) * 0.1f - 0.05f;  // x velocity
            velocity_data[i * 2 + 1] = ((float)rand() / RAND_MAX) * 0.1f - 0.05f;  // y velocity
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ps->position_ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * 2 * max_particles, position_data);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ps->velocity_ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * 2 * max_particles, velocity_data);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        free(position_data);
        free(velocity_data);

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
    
    // Set number of particles
    glUniform1i(glGetUniformLocation(ps->compute_shader, "num_particles"), ps->max_particles);
    
    // Bind SSBOs
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ps->position_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ps->velocity_ssbo);
    
    // Dispatch compute shader
    glDispatchCompute(ps->max_particles / WORK_GROUP_SIZE + 1, 1, 1);
    
    // Wait for compute shader to finish
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

GLuint particle_system_get_position_ssbo(ParticleSystem* ps) {
    return ps->position_ssbo;
}

int particle_system_get_max_particles(ParticleSystem* ps) {
    return ps->max_particles;
}

void particle_system_destroy(ParticleSystem* ps) {
    if (ps) {
        glDeleteBuffers(1, &ps->position_ssbo);
        glDeleteBuffers(1, &ps->velocity_ssbo);
        glDeleteProgram(ps->compute_shader);
        free(ps);
    }
}