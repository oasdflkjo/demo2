#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <stdbool.h>
#include "gl_loader.h"

typedef struct {
    float x, y;        // Position
    float vx, vy;      // Velocity
} Particle;

typedef struct ParticleSystem ParticleSystem;

ParticleSystem* particle_system_create(int max_particles, int width, int height);
void particle_system_update(ParticleSystem* ps, float delta_time, float mouse_x, float mouse_y);
GLuint particle_system_get_ssbo(ParticleSystem* ps);
int particle_system_get_max_particles(ParticleSystem* ps);
void particle_system_destroy(ParticleSystem* ps);

#endif // PARTICLE_SYSTEM_H
