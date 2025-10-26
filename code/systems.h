#pragma once
#include "defs.h"
#include "components.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

typedef struct {
    EntityID ids[5];
    float lifetime[5];
    int count;
    int id_to_index[5];
} LifeTimeSystem;

typedef struct {
    EntityID ids[5];
    glm::vec2 offset[5];
    float lifetime[5];
    int count;
    int id_to_index[5];
} FireMissleSystem;

typedef struct {
    EntityID ids[2];
    int count;
    int id_to_index[2];
} PlayerInputSystem;

typedef struct {
    EntityID ids[5];
    glm::mat4 projection[5];
    glm::mat4 view[5];
    glm::vec3 pos[5];
    bool isLocked[5];
    int count;
    int id_to_index[5];
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
