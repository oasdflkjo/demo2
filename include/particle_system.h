#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <stdbool.h>

typedef struct {
    float x, y;        // Position
    float vx, vy;      // Velocity
    float r, g, b;     // Color
    float lifetime;    // Remaining lifetime
} Particle;

typedef struct ParticleSystem ParticleSystem;

ParticleSystem* particle_system_create(int max_particles);
void particle_system_update(ParticleSystem* ps, float delta_time);
void particle_system_render(ParticleSystem* ps);
void particle_system_destroy(ParticleSystem* ps);

#endif // PARTICLE_SYSTEM_H
