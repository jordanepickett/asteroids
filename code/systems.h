#pragma once
#include "defs.h"
#include "components.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

typedef struct {
    EntityID ids[MAX_ENTITIES];
    int count;
    int id_to_index[MAX_ENTITIES];
} PlayerInputSystem;

typedef struct {
    EntityID ids[MAX_ENTITIES];
    glm::mat4 projection[MAX_ENTITIES];
    glm::mat4 view[MAX_ENTITIES];
    glm::vec3 pos[MAX_ENTITIES];
    bool isLocked[MAX_ENTITIES];
    int count;
    int id_to_index[MAX_ENTITIES];
} CameraSystem;

typedef struct {
    EntityID ids[MAX_ENTITIES];
    glm::vec2 pos[MAX_ENTITIES];
    glm::vec2 rot[MAX_ENTITIES];
    glm::vec2 vel[MAX_ENTITIES];
    int count;
    int id_to_index[MAX_ENTITIES];
} MovementSystem;

typedef struct {
    EntityID ids[MAX_ENTITIES];
    glm::vec2 pos[MAX_ENTITIES];
    glm::vec3 color[MAX_ENTITIES];
    int count;
    int id_to_index[MAX_ENTITIES];
} ModelSystem;

typedef struct {
    EntityID ids[MAX_ENTITIES];
    float hp[MAX_ENTITIES];
    int count;
    int id_to_index[MAX_ENTITIES];
} HealthSystem;

typedef struct {
    EntityID ids[MAX_ENTITIES];
    float damage[MAX_ENTITIES];
    int count;
    int id_to_index[MAX_ENTITIES];
} DamageSystem;

typedef struct {
    CompMask comp[MAX_ENTITIES];
    int active[MAX_ENTITIES];
    int count;
} EntityRegistry;

static void MovementSystemInit(MovementSystem *system); 
static void HealthSystemInit(HealthSystem *system);
static void DamageSystemInit(DamageSystem *system);
static void EntityRegistryInit(EntityRegistry *e);
