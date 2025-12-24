#pragma once
#include "entity.h"
#include "platform.h"
#include "queues.h"
#include "scenes/scene.h"
#include "systems.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <glad/glad.h>

static Vertex SHIP[3] = {
    {{ 0.0f,  1.0f}, {0.1f, 1.f, 0.1f, 1.f}, {0.f, 0.f, 1.f}},
    {{-1.0f, -1.0f}, {0.1f, 1.f, 0.1f, 1.f}, {0.f, 0.f, 1.f}},
    {{ 1.0f, -1.0f}, {0.1f, 1.f, 0.1f, 1.f}, {0.f, 0.f, 1.f}},
};

static Vertex MISSLE[4] = {
    {{ -0.3f,  0.3f}, {1.f, 1.f, 1.f, 1.0f},{0.f, 0.f, 1.f} },
    {{ -0.3f,  -0.3f}, {1.f, 1.f, 1.f, 1.0f}, {0.f, 0.f, 1.f}},
    {{ 0.3f,  -0.3f}, {1.f, 1.f, 1.f, 1.0f}, {0.f, 0.f, 1.f}},
    {{ 0.3f,  0.3f}, {1.f, 1.f, 1.f, 1.0f}, {0.f, 0.f, 1.f}},
};

static Vertex ASTEROID[8] = {
    {{ -0.8f,  0.3f }, {1.f, 1.f, 1.f, 1.0f}, {0.f, 0.f, 1.f}},
    {{ -0.4f,  0.9f }, {1.f, 1.f, 1.f, 1.0f}, {0.f, 0.f, 1.f}},
    {{  0.3f,  1.0f }, {1.f, 1.f, 1.f, 1.0f}, {0.f, 0.f, 1.f}},
    {{  0.9f,  0.3f }, {1.f, 1.f, 1.f, 1.0f}, {0.f, 0.f, 1.f}},
    {{  0.7f, -0.5f }, {1.f, 1.f, 1.f, 1.0f}, {0.f, 0.f, 1.f}},
    {{  0.1f, -1.0f }, {1.f, 1.f, 1.f, 1.0f}, {0.f, 0.f, 1.f}},
    {{ -0.6f, -0.8f }, {1.f, 1.f, 1.f, 1.0f}, {0.f, 0.f, 1.f}},
    {{ -1.0f, -0.2f }, {1.f, 1.f, 1.f, 1.0f}, {0.f, 0.f, 1.f}}
};

typedef struct {
    Scene* scenes[2];
    int count;
} SceneStack;

struct Camera {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 position;
    bool isLocked;
};

typedef struct GameState {
    SceneStack sceneStack;
    void* commands;
    int renderCommandsCount;

    void* soundCommands;
    int soundCommandsCount;

    EntityRegistry *entitiesReg;
    LightSystem *light;
    TextSystem *textSystem;
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
    EmitterSystem *emitter;
    ParticleSystem *particles;
    SoundSystem *sound;
    EventQueue *events;

    ButtonSystem *buttons;
} GameState;

void GameInit(GameState *state, PlatformAPI *platform, PlatformMemory *memory);
void GameUpdate(GameState *state, PlatformFrame *frame, PlatformMemory *memory);
void GameRender(GameState *state, PlatformMemory *memory, PlatformFrame* frame);
void GameSound(GameState *state, PlatformMemory *memory);
Entity* CreateEntity(GameState *state, EntityType entity);
void DestoryEntity(Entity* entity);
void HandleCollision(GameState *state);
void UpdateEntities(GameState* state, PlatformFrame *frame, uint32_t activeSystems);
void PlayerInput(GameState* state, PlatformFrame *frame, Entity* entity);
void ClearGameSystems(GameState* state);
