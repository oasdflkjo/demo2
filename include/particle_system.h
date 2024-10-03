#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <stdbool.h>
#include "window_manager.h"  // Add this include

typedef struct {
    float x, y;        // Position
    float vx, vy;      // Velocity
    float r, g, b;     // Color
    float lifetime;    // Remaining lifetime
} Particle;

typedef struct ParticleSystem ParticleSystem;

ParticleSystem* particle_system_create(int max_particles, WindowManager* window_manager);
void particle_system_update(ParticleSystem* ps, float delta_time, int mouse_x, int mouse_y);
void particle_system_render(ParticleSystem* ps);
void particle_system_destroy(ParticleSystem* ps);

#endif // PARTICLE_SYSTEM_H
