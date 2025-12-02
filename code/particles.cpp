#include "defs.h"
#include "systems.h"
#include <cassert>
#include <cstdio>

void CreateParticle(ParticleSystem *system,
                    glm::vec2 pos, 
                    glm::vec2 vel,
                    float lifetime,
                    glm::vec4 startColor,
                    glm::vec4 endColor) {

    if(system->count >= MAX_PARTICLES) {
        return;
    }
    EntityID id = system->count++;
    //printf("Creating particle: %i\n", id);
    system->active[id] = 1;
    system->pos[id] = pos;
    system->velocity[id] = vel;
    system->lifetime[id] = lifetime;
    system->totalLifetime[id] = lifetime;
    system->color[id] = startColor;
    system->startColor[id] = startColor;
    system->endColor[id] = endColor;
}

static void UpdateColor(ParticleSystem *system, int id) {
    float remaining = system->lifetime[id];
    float total = system->totalLifetime[id];

    float t = 1.0f - (remaining / total);
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    float alpha = 1.0f - t;
    glm::vec4 start = system->startColor[id];
    glm::vec4 end   = system->endColor[id];

    system->color[id] = glm::mix(start, end, t);
    //system->color[id].a = alpha;
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
                system->totalLifetime[i] = system->totalLifetime[last];
                system->color[i] = system->color[last];
                system->startColor[i] = system->startColor[last];
                system->endColor[i] = system->endColor[last];
            }

            // DO NOT increment i here!
        }
        else {
            system->pos[i] += system->velocity[i] * dt;
            UpdateColor(system, i);
            i++;
        }
    }
}
