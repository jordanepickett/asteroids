#include "defs.h"
#include "entity.h"
#include "game.h"
#include "memory.h"
#include "platform.h"
#include "queues.h"
#include "queues.cpp"
#include "render_commands.h"
#include "systems.h"
#include "systems.cpp"
#include <cstdio>
#include <cstring>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

const int MAX_ASTEROIDS = 5;
float asteroidSpawnTimer = 0.0f;
float playerSpawnTimer = 0.0f;

static int CountAsteroids(GameState* state) {
    int count = 0;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (state->entities[i].isActive && 
            state->entities[i].type == ENTITY_ASTEROID)
        {
            count++;
        }
    }
    return count;
}

static void TrySpawnPlayer(GameState* state) {
    bool isAlive = false;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (state->entities[i].isActive && 
            state->entities[i].type == ENTITY_PLAYER)
        {
            isAlive = true;
        } else if(!state->entities[i].isActive &&
            state->entities[i].type == ENTITY_PLAYER){
            isAlive = false;
        }
    }

    if(!isAlive) {
        CreateEntity(state, ENTITY_PLAYER);
    }

}

static void TrySpawnAsteroid(GameState* state) {
    int amount = CountAsteroids(state);
    printf("AMOUNT: %i \n", amount);
    if (amount >= MAX_ASTEROIDS) return;

    while(amount <= MAX_ASTEROIDS) {
        Entity* a = CreateEntity(state, ENTITY_ASTEROID);
        if (a) {
            // random position at edges of screen
            a->transform.position.x = (rand() % 2 == 0) ? 0.0f : 800;
            a->transform.position.y = (float)(rand() % 600);

            // random velocity (pick a random direction + speed)
            float angle = (float)rand() / RAND_MAX * 2.0f * 3.14159f;
            float speed = 5.0f + (rand() % 5); // 50–150 px/sec
            a->transform.velocity.x = cosf(angle) * speed;
            a->transform.velocity.y = sinf(angle) * speed;

            //a->radius = 20.0f + (rand() % 15); // random asteroid size
        }
        amount++;
    }

}

inline bool WasPressed(const ButtonState& newState) {
    return (newState.halfTransitionCount > 0 && newState.endedDown);
}

inline bool WasReleased(const ButtonState& newState) {
    return (newState.halfTransitionCount > 0 && !newState.endedDown);
}

inline bool IsDown(const ButtonState& newState) {
    return newState.endedDown;
}

inline void PushText(MemoryArena* arena, glm::vec2 pos, glm::vec4 color, const char* str) {
    size_t len = strlen(str);
    size_t size = sizeof(RenderCommandDrawText) + len + 1;
    auto* cmd = (RenderCommandDrawText*)ArenaAlloc(arena, size);
    cmd->header.type = RENDER_CMD_DRAW_TEXT;
    cmd->header.size = (uint32_t)size;
    cmd->position = pos;
    cmd->color = color;
    cmd->length = (int)len;
    char* dst = (char*)cmd + sizeof(RenderCommandDrawText);
    memcpy(dst, str, len + 1);
}

inline void PushTrianges2(
    GameState *state,
    MemoryArena *memory,
    Vertex *verts,
    int vertexCount,
    glm::vec2 pos,
    glm::vec2 rot
) {

    glm::mat4 mvp = state->camera.projection * state->camera.view;
    size_t size = sizeof(RenderCommandDrawTriangles) + vertexCount * sizeof(Vertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandDrawTriangles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_DRAW_TRIANGLES;
    drawCmd->header.size = (uint32_t)size;
    drawCmd->mvp = mvp;
    drawCmd->vertexCount = vertexCount;
    drawCmd->pos = pos;
    drawCmd->rotation = rot;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandDrawTriangles);
    memcpy(dst, verts, vertexCount * sizeof(Vertex));
}

inline void PushTrianges(
    GameState *state,
    MemoryArena *memory,
    Vertex *verts,
    int vertexCount,
    Entity* entity
) {

    glm::mat4 mvp = state->camera.projection * state->camera.view;
    size_t size = sizeof(RenderCommandDrawTriangles) + vertexCount * sizeof(Vertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandDrawTriangles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_DRAW_TRIANGLES;
    drawCmd->header.size = (uint32_t)size;
    drawCmd->mvp = mvp;
    drawCmd->vertexCount = vertexCount;
    drawCmd->pos = entity->transform.position;
    drawCmd->rotation = entity->transform.rotation;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandDrawTriangles);
    memcpy(dst, verts, vertexCount * sizeof(Vertex));
}

inline void PushLoop(
    GameState *state,
    MemoryArena *memory,
    Vertex *verts,
    int vertexCount,
    Entity* entity
) {

    glm::mat4 mvp = state->camera.projection * state->camera.view;
    size_t size = sizeof(RenderCommandDrawTriangles) + vertexCount * sizeof(Vertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandDrawTriangles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_DRAW_LOOP;
    drawCmd->header.size = (uint32_t)size;
    drawCmd->pos = entity->transform.position;
    drawCmd->mvp = mvp;
    drawCmd->vertexCount = vertexCount;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandDrawTriangles);
    memcpy(dst, verts, 4 * sizeof(Vertex));
}

inline void UpdateCamera(GameState *state, Entity *entity) {
    if(!state->camera.isLocked) {
        state->camera.position = glm::vec3(-entity->transform.position.x, entity->transform.position.y, 1.0f);
        state->camera.view = glm::lookAt(
            state->camera.position,             // eye
            glm::vec3(-entity->transform.position.x, entity->transform.position.y, 0.0f),        // target
            glm::vec3(0.0f, 1.0f, 0.0f)         // up
        );
    }
}

inline void WrapEntityPosition(Entity *entity) {
    float left   = -20.0f;
    float right  =  20.0f;
    float bottom = -20.0f;
    float top    =  20.0f;

    if (entity->transform.position.x > right)       entity->transform.position.x = left;
    else if (entity->transform.position.x < left)   entity->transform.position.x = right;

    if (entity->transform.position.y > top)         entity->transform.position.y = bottom;
    else if (entity->transform.position.y < bottom) entity->transform.position.y = top;
}

Entity* CreateEntity(GameState* state, EntityType entityType) {
    for(int i = 0; i < MAX_ENTITIES; i++) {
        if(!state->entities[i].isActive) {
            Entity* e = &state->entities[i];
            *e = {};
            e->isActive = true;
            e->type = entityType;
            e->transform = {
                glm::vec2(0.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 0.0f)
            };
            return e;
        }
    }

    return nullptr;
}

void DestoryEntity(Entity *entity) {
    entity->isActive = false;
}

void HandleCollision(GameState *state) {
    for(int i = 0; i < MAX_ENTITIES; i++) {
        Entity* a = &state->entities[i];
        if(!a->isActive) continue;

        for(int j = i + 1; j < MAX_ENTITIES; j++) {
            Entity* b = &state->entities[j];
            if(!b->isActive) continue;

            float dx = a->transform.position.x - b->transform.position.x;
            float dy = a->transform.position.y - b->transform.position.y;
            float dist2 = dx*dx + dy*dy;

            float r = a->radius + b->radius;

            if(dist2 < r*r) {
                if((a->type == ENTITY_MISSLE && b->type == ENTITY_ASTEROID) ||
                    (a->type == ENTITY_ASTEROID && b->type == ENTITY_MISSLE)) {
                    DestoryEntity(a);
                    DestoryEntity(b);
                }

                if((a->type == ENTITY_PLAYER && b->type == ENTITY_ASTEROID) ||
                    (a->type == ENTITY_ASTEROID && b->type == ENTITY_PLAYER)) {
                    DestoryEntity(a);
                    DestoryEntity(b);
                }
            }
        }
    }
}

void GameInit(GameState *state, PlatformAPI *platform, PlatformMemory *memory) {
    
    state->entitiesReg = (EntityRegistry*)ArenaAlloc(&memory->permanent, sizeof(EntityRegistry));
    state->movement = (MovementSystem*)ArenaAlloc(&memory->permanent, sizeof(MovementSystem));
    state->health = (HealthSystem*)ArenaAlloc(&memory->permanent, sizeof(HealthSystem));
    state->damage = (DamageSystem*)ArenaAlloc(&memory->permanent, sizeof(DamageSystem));
    //state->render = ARENA_PUSH_STRUCT(arena, RenderQueue);
    state->collisions = (CollisionQueue*)ArenaAlloc(&memory->permanent, sizeof(CollisionQueue));
    state->playerInput = (PlayerInputSystem*)ArenaAlloc(&memory->permanent, sizeof(PlayerInputSystem));


    memset(state->entitiesReg, 0, sizeof(EntityRegistry));
    memset(state->movement, 0, sizeof(MovementSystem));
    memset(state->health, 0, sizeof(HealthSystem));
    memset(state->damage, 0, sizeof(DamageSystem));
    memset(state->collisions, 0, sizeof(CollisionQueue));
    memset(state->playerInput, 0, sizeof(PlayerInputSystem));

    EntityID player = CreateEntity2(state);
    glm::vec2 zero = { 0, 0};
    glm::vec2 rot = { 0, 1};
    AddMovement(state, player, zero, rot, zero);
    AddPlayerInput(state, player);

    //EntityRegistryInit(state->entitiesReg);
    //MovementSystemInit(state->movement);
    //HealthSystemInit(state->health);
    //DamageSystemInit(state->damage);
    //RenderQueueInit(g->render);
    //CollisionQueueInit(state->collisions);

    float left   = -20.0f;
    float right  =  20.0f;
    float bottom = -20.0f;
    float top    =  20.0f;
    float nearZ  = -1.0f;
    float farZ   =  1.0f;

    state->camera.projection = glm::ortho(left, right, bottom, top, nearZ, farZ);

    // Place camera at z=1 looking at the origin
    state->camera.position = glm::vec3(0.0f, 0.0f, 1.0f);
    state->camera.view = glm::lookAt(
        state->camera.position,             // eye
        glm::vec3(0.0f, 0.0f, 0.0f),        // target
        glm::vec3(0.0f, 1.0f, 0.0f)         // up
    );
    state->camera.isLocked = true;
    //state->player.rotation = glm::vec2(0.0f, 1.0f);

    state->entities = (Entity*)ArenaAlloc(&memory->permanent, sizeof(Entity) * MAX_ENTITIES);
    //state->freeEntityCount = MAX_ENTITIES;
    for(int i = 0; i < MAX_ENTITIES; i++) {
        //state->freeEntitiesList[i] = MAX_ENTITIES - 1 - i;
        
        state->entities[i] = {};
        state->entities[i].isActive = false;
        state->entities[i].type = ENTITY_NONE;
    }

    //CreateEntity(state, ENTITY_PLAYER);
}

void GameUpdate(GameState *state, PlatformFrame *frame, PlatformMemory *memory) {

    asteroidSpawnTimer -= frame->deltaTime;
    if (asteroidSpawnTimer <= 0.0f) {
        TrySpawnAsteroid(state);
        asteroidSpawnTimer = 3.0f; // spawn every ~3 seconds
    }

    playerSpawnTimer -= frame->deltaTime;
    if (playerSpawnTimer <= 0.0f) {
        TrySpawnPlayer(state);
        playerSpawnTimer = 5.0f;
    }

    UpdateEntities(state, frame);
    GameRender(state, memory);
    HandleCollision(state);
}

void PlayerInput(GameState* state, PlatformFrame *frame, Entity* entity) {
    float rightBurst = 0;
    float leftBurst = 0;
    float lx = 0;
    float ly = 0;
    if(WasPressed(frame->input.controllers[0].actionDown)) {
        Entity* e = CreateEntity(state, ENTITY_MISSLE);
        if(e) {
            e->lifeTime = 0.5f;
            e->transform.position.x = entity->transform.position.x + entity->transform.rotation.x;
            e->transform.position.y = entity->transform.position.y + entity->transform.rotation.y;

            e->transform.rotation.x = entity->transform.rotation.x;
            e->transform.rotation.y = entity->transform.rotation.y;

            e->transform.velocity = entity->transform.rotation * 50.0f;
        }
    } 
    if(frame->input.controllers[0].actionDown.endedDown) {
    } 
    if(frame->input.controllers[0].moveDown.endedDown) {
        //state->player.pos.y -= 0.02f;
    }
    if(frame->input.controllers[0].moveUp.endedDown) {
        //state->player.pos.y += 0.02f;
    }
    if(frame->input.controllers[0].moveLeft.endedDown) {
        //state->player.pos.x -= 0.02f;
    }
    if(frame->input.controllers[0].moveRight.endedDown) {
        //state->player.pos.x += 0.02f;
    }
    if(frame->input.controllers[0].rightShoulder.endedDown) {
        rightBurst = 1;
    } else {
        rightBurst = 0;
    }
    
    lx = frame->input.controllers[0].stickAverageX;
    ly = frame->input.controllers[0].stickAverageY;

    float acceleration = 5.0f;
    float maxSpeed     = 20.0f;
    float damping      = 0.992f;
    float rotLerp   = 15.0f;

    glm::vec2 input(lx, ly);

    input.y = -input.y;
    input.x = -input.x;

    if(glm::length(input) > 0.0f) {
        entity->transform.rotation = glm::normalize(glm::mix(entity->transform.rotation, input, rotLerp * frame->deltaTime));
    }

    if(rightBurst) {
        entity->transform.velocity += entity->transform.rotation * acceleration;
    }

    if (glm::length(entity->transform.velocity) > maxSpeed) {
        entity->transform.velocity = glm::normalize(entity->transform.velocity) * maxSpeed;
    }

    entity->transform.velocity *= damping;

    entity->transform.position += entity->transform.velocity * frame->deltaTime;
}

void UpdateEntities(GameState *state, PlatformFrame *frame) {

    PlayerInputUpdate(state->playerInput, state->movement, frame);

    for(int i = 0; i < MAX_ENTITIES; i++) {
        Entity* e = &state->entities[i];

        if(!e->isActive) continue;

        switch(e->type) {
            case ENTITY_PLAYER:
                {
                    //PlayerInput(state, frame, e);
                    UpdateCamera(state, e);
                } break;
            case ENTITY_ASTEROID:
                {
                    e->transform.position += e->transform.velocity * frame->deltaTime;
                } break;
            case ENTITY_MISSLE:
                {
                    e->transform.position += e->transform.velocity * frame->deltaTime;
                    e->lifeTime -= 1.0f * frame->deltaTime;
                    if (e->lifeTime <= 0) {
                        e->isActive = false;
                    }
                } break;
            default:
                {

                } break;
        }

        if(state->camera.isLocked) {
            WrapEntityPosition(e);
        }
    }
}

void GameRender(GameState *state, PlatformMemory *memory) {
    ArenaReset(&memory->transient);

    // Clear
    auto* cmd = (RenderCommandClear*)ArenaAlloc(&memory->transient, sizeof(RenderCommandClear));
    cmd->header.type = RENDER_CMD_CLEAR;
    cmd->header.size = sizeof(RenderCommandClear);
    cmd->color = {0.f, 0.f, 0.f};

    MovementSystem *movementSystem = state->movement;

    for(int i = 0; i < state->entitiesReg->count; i++) {
        int movementIndex = movementSystem->id_to_index[i];
        glm::vec2 pos = {0,0};
        glm::vec2 rot = {0,0};
        if (movementIndex >= 0) {
            pos = movementSystem->pos[movementIndex];
            rot = movementSystem->rot[movementIndex];
        } 

        Vertex verts[3] = {
            {{ 0.0f,  1.0f}, {0.f, 1.f, 0.f}},
            {{-1.0f, -1.0f}, {0.f, 1.f, 0.f}},
            {{ 1.0f, -1.0f}, {0.f, 1.f, 0.f}},
        };
        PushTrianges2(state, &memory->transient, verts, 3, pos, rot);

    }

    for(int i = 0; i < MAX_ENTITIES; i++) {
        Entity* e = &state->entities[i];

        if(!e->isActive) continue;

        switch(e->type) {
            case ENTITY_PLAYER:
                {
                    Vertex verts[3] = {
                        {{ 0.0f,  1.0f}, {0.f, 1.f, 0.f}},
                        {{-1.0f, -1.0f}, {0.f, 1.f, 0.f}},
                        {{ 1.0f, -1.0f}, {0.f, 1.f, 0.f}},
                    };
                    //PushTrianges(state, &memory->transient, verts, 3, e);
                } break;
            case ENTITY_ASTEROID:
                {
                    Vertex verts[4] = {
                        {{ -1.0f,  1.0f}, {1.f, 1.f, 1.f}},
                        {{ -1.0f,  -1.0f}, {1.f, 1.f, 1.f}},
                        {{ 1.0f,  -1.0f}, {1.f, 1.f, 1.f}},
                        {{ 1.0f,  1.0f}, {1.f, 1.f, 1.f}},
                    };
                    PushLoop(state, &memory->transient, verts, 4, e);
                } break;
            case ENTITY_MISSLE:
                {
                    Vertex verts[4] = {
                        {{ -0.3f,  0.3f}, {1.f, 1.f, 1.f}},
                        {{ -0.3f,  -0.3f}, {1.f, 1.f, 1.f}},
                        {{ 0.3f,  -0.3f}, {1.f, 1.f, 1.f}},
                        {{ 0.3f,  0.3f}, {1.f, 1.f, 1.f}},
                    };
                    PushLoop(state, &memory->transient, verts, 4, e);
                } break;
            default: 
                {

                } break;
        }
    }

    //{
    //    Vertex verts[4] = {
    //        {{ 0.0f,  0.0f}, {1.f, 0.f, 0.f}},
    //        {{ 0.0f, -1.0f}, {0.f, 1.f, 0.f}},
    //        {{ 1.0f, -1.0f}, {0.f, 0.f, 1.f}},
    //        {{ 1.0f,  0.0f}, {0.f, 0.f, 1.f}},
    //    };
    //    PushLoop(state, &memory->transient, verts, 4);
    //}

    // TODO: Text
    //PushText(&memory->transient, {20, 20}, {1,1,1,1}, "Health: 100");

    state->commands = memory->transient.base;
    state->renderCommandsCount = memory->transient.used;
}

