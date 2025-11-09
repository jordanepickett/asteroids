#pragma once
#include "entity.h"
#include "platform.h"
#include "queues.h"
#include "systems.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

static Vertex SHIP[3] = {
    {{ 0.0f,  1.0f}, {0.f, 1.f, 0.f}},
    {{-1.0f, -1.0f}, {0.f, 1.f, 0.f}},
    {{ 1.0f, -1.0f}, {0.f, 1.f, 0.f}},
};

static Vertex MISSLE[4] = {
    {{ -0.3f,  0.3f}, {1.f, 1.f, 1.f}},
    {{ -0.3f,  -0.3f}, {1.f, 1.f, 1.f}},
    {{ 0.3f,  -0.3f}, {1.f, 1.f, 1.f}},
    {{ 0.3f,  0.3f}, {1.f, 1.f, 1.f}},
};

static Vertex ASTEROID[4] = {
    {{ -1.0f,  1.0f}, {1.f, 1.f, 1.f}},
    {{ -1.0f,  -1.0f}, {1.f, 1.f, 1.f}},
    {{ 1.0f,  -1.0f}, {1.f, 1.f, 1.f}},
    {{ 1.0f,  1.0f}, {1.f, 1.f, 1.f}},
};

struct Camera {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 position;
    bool isLocked;
};

typedef struct {
    GLuint vao;

    void* commands;
    int renderCommandsCount;
    Entity *entities;
    int *freeEntitiesList;
    int freeEntityCount;
    Camera camera;

    EntityRegistry *entitiesReg;
    MovementSystem *movement;
    RenderSystem *render;
    HealthSystem *health;
    DamageSystem *damage;
    CameraSystem *cameraSys;
    CollisionQueue *collisions;
    ProjectileQueue *projectile;
    PlayerInputSystem *playerInput;
    FireMissleSystem *fireMissile;
    LifeTimeSystem *lifetime;
    FloatableSystem *floatable;
    CollisionSystem *collision;
} GameState;

void GameInit(GameState *state, PlatformAPI *platform, PlatformMemory *memory);
void GameUpdate(GameState *state, PlatformFrame *frame, PlatformMemory *memory);
void GameRender(GameState *state, PlatformMemory *memory);
Entity* CreateEntity(GameState *state, EntityType entity);
void DestoryEntity(Entity* entity);
void HandleCollision(GameState *state);
void UpdateEntities(GameState* state, PlatformFrame *frame);
void PlayerInput(GameState* state, PlatformFrame *frame, Entity* entity);
