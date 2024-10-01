#include "particle_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <gl/gl.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct ParticleSystem {
    Particle* particles;
    int max_particles;
    int active_particles;
};

ParticleSystem* particle_system_create(int max_particles) {
    ParticleSystem* ps = (ParticleSystem*)malloc(sizeof(ParticleSystem));
    if (ps) {
        ps->particles = (Particle*)malloc(sizeof(Particle) * max_particles);
        ps->max_particles = max_particles;
        ps->active_particles = max_particles;  // Start with all particles active
        
        for (int i = 0; i < max_particles; i++) {
            ps->particles[i].x = (float)(rand() % WINDOW_WIDTH) / WINDOW_WIDTH * 2 - 1;
            ps->particles[i].y = (float)(rand() % WINDOW_HEIGHT) / WINDOW_HEIGHT * 2 - 1;
            ps->particles[i].vx = ((float)rand() / RAND_MAX - 0.5f) * 0.02f;
            ps->particles[i].vy = ((float)rand() / RAND_MAX - 0.5f) * 0.02f;
            ps->particles[i].r = (float)rand() / RAND_MAX;
            ps->particles[i].g = (float)rand() / RAND_MAX;
            ps->particles[i].b = (float)rand() / RAND_MAX;
            ps->particles[i].lifetime = (float)rand() / RAND_MAX * 5.0f;  // 0-5 seconds lifetime
        }
    }
    return ps;
}

void particle_system_update(ParticleSystem* ps, float delta_time) {
    for (int i = 0; i < ps->max_particles; i++) {
        Particle* p = &ps->particles[i];
        
        p->x += p->vx * delta_time;
        p->y += p->vy * delta_time;
        p->lifetime -= delta_time;

        // Reset particle if it's dead or out of bounds
        if (p->lifetime <= 0 || fabs(p->x) > 1 || fabs(p->y) > 1) {
            p->x = (float)(rand() % WINDOW_WIDTH) / WINDOW_WIDTH * 2 - 1;
            p->y = (float)(rand() % WINDOW_HEIGHT) / WINDOW_HEIGHT * 2 - 1;
            p->vx = ((float)rand() / RAND_MAX - 0.5f) * 0.02f;
            p->vy = ((float)rand() / RAND_MAX - 0.5f) * 0.02f;
            p->lifetime = (float)rand() / RAND_MAX * 5.0f;
        }
    }
}

void particle_system_render(ParticleSystem* ps) {
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < ps->max_particles; i++) {
        Particle* p = &ps->particles[i];
        glColor3f(p->r, p->g, p->b);
        glVertex2f(p->x, p->y);
    }
    glEnd();
}

void particle_system_destroy(ParticleSystem* ps) {
    if (ps) {
        free(ps->particles);
        free(ps);
    }
}
