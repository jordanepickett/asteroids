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
