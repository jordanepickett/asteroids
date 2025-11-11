#pragma once
#include "defs.h"
#include "components.h"
#include "tags.h"
#include "entity.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

typedef struct {
    EntityID ids[MAX_LIFETIMES];
    float lifetime[MAX_LIFETIMES];
    int count;
    int id_to_index[MAX_LIFETIMES];
} LifeTimeSystem;

typedef struct {
    EntityID ids[20];
    glm::vec2 offset[20];
    float lifetime[20];
    int count;
    int id_to_index[20];
} FireMissleSystem;

typedef struct {
    EntityID ids[3];
    int id_to_index[3];
    int count;
} PlayerInputSystem;

typedef struct {
    EntityID ids[3];
    glm::mat4 projection[3];
    glm::mat4 view[3];
    glm::vec3 pos[3];
    bool isLocked[3];
    int count;
    int id_to_index[3];
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
    Vertex* verts[MAX_ENTITIES];
    int vertCount[MAX_ENTITIES];
    int count;
    int id_to_index[MAX_ENTITIES];
} RenderSystem;

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
    EntityID ids[MAX_FLOATABLES];
    int count;
    int id_to_index[MAX_FLOATABLES];
} FloatableSystem;

typedef struct {
    EntityID ids[MAX_ENTITIES];
    float size[MAX_ENTITIES];
    int count;
    int id_to_index[MAX_ENTITIES];
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
