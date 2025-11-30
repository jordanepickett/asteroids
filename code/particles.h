#pragma once

#include "systems.h"
#include <glm/glm.hpp>

typedef enum {
    EMITTER_ONESHOT = 1 << 0,      // Spawn all particles immediately
    EMITTER_CONTINUOUS = 1 << 1,    // Spawn over time
    EMITTER_FOLLOW_ENTITY = 1 << 2, // Update position from parent
    EMITTER_INHERIT_VELOCITY = 1 << 3, // Add parent velocity to particles
    EMITTER_EXPIRE_ON_DEATH = 1 << 4,  // Auto-destroy when parent dies
} EmitterFlags;

typedef enum {
    EMITTER_DEATH_EXPLOSION,
    EMITTER_TRAIL,
    EMITTER_TORCH
} EmitterIDs;

typedef struct {
    uint8_t id;
    float lifetime;
    float spawnRate;
    uint16_t burstCount;
    glm::vec2 velocityBase;
    glm::vec2 velocityVariance;
    glm::vec4 colorStart;
    glm::vec4 colorEnd;
    float sizeStart;
    float sizeEnd;
    uint8_t flags;
} ParticleEmitterPreset;

/*
static ParticleEmitterPreset EmitterPresets[256] = {
    [EMITTER_DEATH_EXPLOSION] = {
        .id = EMITTER_DEATH_EXPLOSION,
        .lifetime = 1.0f,
        .burstCount = 30,
        .velocityBase = {0, 0},
        .velocityVariance = {100, 100},
        .colorStart = {1.0f, 0.5f, 0.0f, 1.0f},
        .colorEnd = {0.3f, 0.0f, 0.0f, 0.0f},
        .sizeStart = 8.0f,
        .sizeEnd = 2.0f,
        .flags = EMITTER_ONESHOT
    },
    
    [EMITTER_TRAIL] = {
        .id = EMITTER_TRAIL,
        .lifetime = 0.5f,
        .spawnRate = 50.0f,  // 50 particles per second
        .velocityBase = {0, 0},
        .velocityVariance = {10, 10},
        .colorStart = {0.0f, 1.0f, 1.0f, 0.8f},
        .colorEnd = {0.0f, 0.5f, 1.0f, 0.0f},
        .sizeStart = 4.0f,
        .sizeEnd = 1.0f,
        .flags = EMITTER_CONTINUOUS | EMITTER_FOLLOW_ENTITY
    },
    
    [EMITTER_TORCH] = {
        .id = EMITTER_TORCH,
        .lifetime = 1.5f,
        .spawnRate = 20.0f,
        .velocityBase = {0, 50},  // Upward
        .velocityVariance = {15, 10},
        .colorStart = {1.0f, 0.8f, 0.0f, 1.0f},
        .colorEnd = {1.0f, 0.0f, 0.0f, 0.0f},
        .sizeStart = 6.0f,
        .sizeEnd = 2.0f,
        .flags = EMITTER_CONTINUOUS
    }
};
*/

void CreateParticle(ParticleSystem *system,
                    glm::vec2 pos, 
                    glm::vec2 vel,
                    float lifetime,
                    glm::vec4 color);


void UpdateParticles(ParticleSystem *system, float dt);
