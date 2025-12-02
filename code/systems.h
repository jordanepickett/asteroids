#pragma once
#include "defs.h"
#include "components.h"
#include "render_commands.h"
#include "tags.h"
#include "entity.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

typedef struct {
    float lifetime[MAX_ENTITIES];
    unsigned char present[MAX_ENTITIES];
} LifeTimeSystem;

typedef struct {
    glm::vec2 offset[20];
    float lifetime[20];
    unsigned char present[MAX_ENTITIES];
} FireMissleSystem;

typedef struct {
    glm::vec2 pos[20];
    glm::vec4 color[20];
    Anchor anchor[20];
    EntityID source[20];
    FieldType fieldType[20];
    unsigned char present[20];
} TextSystem;

typedef struct {
    glm::vec2 pos[MAX_PARTICLES];
    glm::vec2 velocity[MAX_PARTICLES];
    glm::vec4 color[MAX_PARTICLES];
    glm::vec4 startColor[MAX_PARTICLES];
    glm::vec4 endColor[MAX_PARTICLES];
    float totalLifetime[MAX_PARTICLES];
    float lifetime[MAX_PARTICLES];
    float size[MAX_PARTICLES];
    unsigned char active[MAX_PARTICLES];
    unsigned char present[MAX_PARTICLES];
    int count;
} ParticleSystem;

typedef struct {
    glm::vec2 pos[MAX_EMITTERS];
    glm::vec2 spawnVelocityBase[MAX_EMITTERS];
    glm::vec2 spawnVelocityVariance[MAX_EMITTERS];
    float spawnRate[MAX_EMITTERS];         // Particles per second
    float spawnTimer[MAX_EMITTERS];        // Accumulator
    float particleLifetime[MAX_EMITTERS];
    glm::vec4 startColor[MAX_EMITTERS];
    glm::vec4 endColor[MAX_EMITTERS];
    float startSize[MAX_EMITTERS];
    float endSize[MAX_EMITTERS];
    EntityID parentEntity[MAX_EMITTERS];   // Optional entity to follow
    unsigned char present[MAX_EMITTERS];
} EmitterSystem;

typedef struct {
    unsigned char present[MAX_ENTITIES];
} PlayerInputSystem;

typedef struct {
    glm::mat4 projection[3];
    glm::mat4 view[3];
    glm::vec3 pos[3];
    bool isLocked[3];
    bool isActive[3];
    unsigned char present[3];
} CameraSystem;

typedef struct {
    glm::vec2 pos[16];
    glm::vec3 color[16];
    float radius[16];
    float intesity[16];
    EntityID source[16];
    unsigned char present[16];
} LightSystem;

typedef struct {
    glm::vec2 pos[MAX_ENTITIES];
    glm::vec2 rot[MAX_ENTITIES];
    glm::vec2 vel[MAX_ENTITIES];
    unsigned char present[MAX_ENTITIES];
} MovementSystem;

typedef struct {
    Vertex* verts[MAX_ENTITIES];
    int vertCount[MAX_ENTITIES];
    unsigned char present[MAX_ENTITIES];
} RenderSystem;

typedef struct {
    glm::vec2 pos[MAX_ENTITIES];
    glm::vec3 color[MAX_ENTITIES];
    unsigned char present[MAX_ENTITIES];
} ModelSystem;

typedef struct {
    float currentHP[MAX_ENTITIES];
    float maxHP[MAX_ENTITIES];
    unsigned char present[MAX_ENTITIES];
} HealthSystem;

typedef struct {
    float damage[MAX_ENTITIES];
    TagMask tags[MAX_ENTITIES];
    unsigned char present[MAX_ENTITIES];
} DamageSystem;

typedef struct {
    unsigned char present[MAX_ENTITIES];
} FloatableSystem;

typedef struct {
    float size[MAX_ENTITIES];
    unsigned char present[MAX_ENTITIES];
} CollisionSystem;

typedef struct {
    EntityID freeList[MAX_ENTITIES];
    int freeCount;
    CompMask comp[MAX_ENTITIES];
    TagMask tag[MAX_ENTITIES];
    int active[MAX_ENTITIES];
    int toDelete[MAX_ENTITIES];
    int deleteCount;
    int count;
} EntityRegistry;
