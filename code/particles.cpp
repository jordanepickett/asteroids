#include "defs.h"
#include "systems.h"
#include <cassert>
#include <cstdio>

void CreateParticle(ParticleSystem *system,
                    glm::vec2 pos, 
                    glm::vec2 vel,
                    float lifetime,
                    glm::vec4 color) {

    if(system->count >= MAX_PARTICLES) {
        return;
    }
    EntityID id = system->count++;
    printf("Creating particle: %i\n", id);
    system->active[id] = 1;
    system->pos[id] = pos;
    system->velocity[id] = vel;
    system->lifetime[id] = lifetime;
    system->color[id] = color;
}

void UpdateParticles(ParticleSystem *system, float dt) {
    int i = 0;

    while(i < system->count)
    {
        system->lifetime[i] -= dt;

        if(system->lifetime[i] <= 0.0f)
        {
            // swap-delete
            int last = --system->count;

            if(i != last)
            {
                system->pos[i] = system->pos[last];
                system->velocity[i] = system->velocity[last];
                system->lifetime[i] = system->lifetime[last];
                system->color[i] = system->color[last];
            }

            // DO NOT increment i here!
        }
        else {
            system->pos[i] += system->velocity[i] * dt;
            i++;
        }
    }
}
