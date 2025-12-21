#include "defs.h"
#include "entity.h"
#include "game.h"
#include "scenes/scene_start.h"
#include "scenes/scene_game.h"
#include "audio/audio_commands.h"
#include "memory.h"
#include "queues.h"
#include "queues.cpp"
#include "render_commands.h"
#include "systems.h"
#include "particles.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

inline void PushText(GameState* state, MemoryArena* arena, glm::vec2 pos, glm::vec4 color, const char* str, Anchor anchor) {
    size_t len = strlen(str);
    size_t size = sizeof(RenderCommandDrawText) + len + 1;
    auto* cmd = (RenderCommandDrawText*)ArenaAlloc(arena, size);
    cmd->header.type = RENDER_CMD_DRAW_TEXT;
    cmd->header.size = (uint32_t)size;
    cmd->position = pos;
    cmd->color = color;
    cmd->length = (int)len;
    cmd->anchor = anchor;
    char* dst = (char*)cmd + sizeof(RenderCommandDrawText);
    memcpy(dst, str, len + 1);
}

static void PushTextf(GameState* state, MemoryArena* arena, glm::vec2 pos, glm::vec4 color, Anchor anchor, const char* fmt, ...) {
    char buffer[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    PushText(state, arena, pos, color, buffer, anchor);
}

inline void PushParticleBatch(
    glm::mat4 mvp,
    MemoryArena *memory,
    ParticleVertex *verts,
    int vertexCount
) {

    size_t size = sizeof(RenderCommandBatchParticles) + vertexCount * sizeof(ParticleVertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandBatchParticles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_BATCH_PARTICLES;
    drawCmd->header.size = (uint32_t)size;
    drawCmd->mvp = mvp;
    drawCmd->vertexCount = vertexCount;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandBatchParticles);
    memcpy(dst, verts, vertexCount * sizeof(ParticleVertex));
}

inline void PushTrianges2(
    glm::mat4 mvp,
    MemoryArena *memory,
    Vertex *verts,
    int vertexCount,
    glm::vec2 pos,
    glm::vec2 rot
) {

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

inline void PushLoop2(
    glm::mat4 mvp,
    MemoryArena *memory,
    Vertex *verts,
    int vertexCount,
    glm::vec2 pos,
    glm::vec2 rot
) {

    size_t size = sizeof(RenderCommandDrawTriangles) + vertexCount * sizeof(Vertex); // assuming simple 2D verts
    auto* drawCmd = (RenderCommandDrawTriangles*)ArenaAlloc(memory, size);
    drawCmd->header.type = RENDER_CMD_DRAW_LOOP;
    drawCmd->header.size = (uint32_t)size;
    drawCmd->pos = pos;
    drawCmd->rotation = rot;
    drawCmd->mvp = mvp;
    drawCmd->vertexCount = vertexCount;
    void* dst = (uint8_t*)drawCmd + sizeof(RenderCommandDrawTriangles);
    memcpy(dst, verts, vertexCount * sizeof(Vertex));
}

inline void WrapSystem(GameState* state) {
    float left   = -20.0f;
    float right  =  20.0f;
    float bottom = -20.0f;
    float top    =  20.0f;

    MovementSystem* movementSystem = state->movement;
    for(int i = 0; i < state->entitiesReg->count; i++) {
        if(!(state->entitiesReg->comp[i] & COMP_MOVEMENT)) {
            continue;
        }
        glm::vec2 pos = movementSystem->pos[i];
        if (pos.x > right)       pos.x = left;
        else if (pos.x < left)   pos.x = right;

        if (pos.y > top)         pos.y = bottom;
        else if (pos.y < bottom) pos.y = top;

        movementSystem->pos[i] = pos;
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

void DestoryEntity(Entity *entity) {
    entity->isActive = false;
}

void ClearGameSystems(GameState *state) {
    // Systems 0
    memset(state->entitiesReg, 0, sizeof(EntityRegistry));
    memset(state->emitter, 0, sizeof(EmitterSystem));
    memset(state->particles, 0, sizeof(ParticleSystem));
    memset(state->render, 0, sizeof(RenderSystem));
    memset(state->light, 0, sizeof(LightSystem));
    memset(state->textSystem, 0, sizeof(TextSystem));
    memset(state->movement, 0, sizeof(MovementSystem));
    memset(state->health, 0, sizeof(HealthSystem));
    memset(state->damage, 0, sizeof(DamageSystem));
    memset(state->cameraSys, 0, sizeof(CameraSystem));
    memset(state->playerInput, 0, sizeof(PlayerInputSystem));
    memset(state->fireMissile, 0, sizeof(FireMissleSystem));
    memset(state->lifetime, 0, sizeof(LifeTimeSystem));
    memset(state->floatable, 0, sizeof(FloatableSystem));
    memset(state->collision, 0, sizeof(CollisionSystem));
    memset(state->sound, 0, sizeof(SoundSystem));

    // Queues 0
    memset(state->collisions, 0, sizeof(CollisionQueue));
    memset(state->projectile, 0, sizeof(ProjectileQueue));
    memset(state->events, 0, sizeof(EventQueue));
}

void GameInit(GameState *state, PlatformAPI *platform, PlatformMemory *memory) {
    
    // Systems
    state->entitiesReg = (EntityRegistry*)ArenaAlloc(&memory->permanent, sizeof(EntityRegistry));
    state->emitter = (EmitterSystem*)ArenaAlloc(&memory->permanent, sizeof(EmitterSystem));
    state->particles = (ParticleSystem*)ArenaAlloc(&memory->permanent, sizeof(ParticleSystem));
    state->render = (RenderSystem*)ArenaAlloc(&memory->permanent, sizeof(RenderSystem));
    state->light = (LightSystem*)ArenaAlloc(&memory->permanent, sizeof(LightSystem));
    state->textSystem = (TextSystem*)ArenaAlloc(&memory->permanent, sizeof(TextSystem));
    state->movement = (MovementSystem*)ArenaAlloc(&memory->permanent, sizeof(MovementSystem));
    state->health = (HealthSystem*)ArenaAlloc(&memory->permanent, sizeof(HealthSystem));
    state->damage = (DamageSystem*)ArenaAlloc(&memory->permanent, sizeof(DamageSystem));
    state->cameraSys = (CameraSystem*)ArenaAlloc(&memory->permanent, sizeof(CameraSystem));
    state->playerInput = (PlayerInputSystem*)ArenaAlloc(&memory->permanent, sizeof(PlayerInputSystem));
    state->fireMissile = (FireMissleSystem*)ArenaAlloc(&memory->permanent, sizeof(FireMissleSystem));
    state->lifetime = (LifeTimeSystem*)ArenaAlloc(&memory->permanent, sizeof(LifeTimeSystem));
    state->floatable = (FloatableSystem*)ArenaAlloc(&memory->permanent, sizeof(FloatableSystem));
    state->collision = (CollisionSystem*)ArenaAlloc(&memory->permanent, sizeof(CollisionSystem));
    state->sound = (SoundSystem*)ArenaAlloc(&memory->permanent, sizeof(SoundSystem));

    // Queues
    state->collisions = (CollisionQueue*)ArenaAlloc(&memory->permanent, sizeof(CollisionQueue));
    state->projectile = (ProjectileQueue*)ArenaAlloc(&memory->permanent, sizeof(ProjectileQueue));
    state->events = (EventQueue*)ArenaAlloc(&memory->permanent, sizeof(EventQueue));

    ClearGameSystems(state);

    //EntityRegistryInit(state->entitiesReg);
    //LifetimeSystemInit(state->lifetime);
    //MovementSystemInit(state->movement);
    //HealthSystemInit(state->health);
    //DamageSystemInit(state->damage);
    //RenderSystemInit(state->render);
    //CollisionSystemInit(state->collision);
    //RenderQueueInit(g->render);
    //CollisionQueueInit(state->collisions);

    SceneStack sceneStack = {};
    sceneStack.scenes[sceneStack.count] = &SceneStart;
    sceneStack.count++;
    //sceneStack.scenes[sceneStack.count] = &SceneGame;
    //sceneStack.count++;
    state->sceneStack = sceneStack;

    sceneStack.scenes[0]->onEnter(state);
}

void GameUpdate(GameState *state, PlatformFrame *frame, PlatformMemory *memory) {

    /*
    asteroidSpawnTimer -= frame->deltaTime;
    if (asteroidSpawnTimer <= 0.0f) {
        TrySpawnAsteroid(state);
        asteroidSpawnTimer = 3.0f; // spawn every ~3 seconds
    }
    */
    uint32_t activeSystems = 0;

    SceneStack* stack = &state->sceneStack;

    for (int i = stack->count - 1; i >= 0; i--) {
        Scene* s = stack->scenes[i];
        activeSystems |= s->systemMask;
        //s->update(state, frame, memory);

        if (s->blocksUpdate)
            break;
    } 

    UpdateEntities(state, frame, activeSystems);
    state->sceneStack.scenes[stack->count - 1]->update(state, frame, memory);
    //GameRender(state, memory, frame);
}

void UpdateEntities(GameState *state, PlatformFrame *frame, uint32_t activeSystems) {
    //printf("entity count: %i\n", state->entitiesReg->count);

    // Inputs
    if(activeSystems & SYS_INPUT) {
        PlayerInputUpdate(state, frame);
    }
    FireMissleUpdate(state, frame);

    // Queue processors
    ProcessProjectileFire(state);

    // Systems
    EmitterUpdate(state, frame->deltaTime);
    if(activeSystems & SYS_MOVE) {
        MovementUpdate(state, frame);
        LifeTimeUpdate(state, frame);
    }

    CollisionUpdate(state, state->collisions);
    ProcessCollisions(state);


    ProcessEvents(state);
    UpdateParticles(state->particles, frame->deltaTime);

    //if(state->camera.isLocked) {
        WrapSystem(state);
    //}

    // Cleanup
    CleanupDeadEntities(state);

}

static void RenderEntities(GameState *state, PlatformMemory *memory, PlatformFrame* frame, uint32_t renderSystems) {

    ArenaReset(&memory->transient);

    // Clear
    auto* cmd = (RenderCommandClear*)ArenaAlloc(&memory->transient, sizeof(RenderCommandClear));
    cmd->header.type = RENDER_CMD_CLEAR;
    cmd->header.size = sizeof(RenderCommandClear);
    cmd->color = {0.f, 0.f, 0.f};

    MovementSystem *movementSystem = state->movement;
    RenderSystem *renderSystem = state->render;
    CameraSystem *cameraSystem = state->cameraSys;
    TextSystem *textSystem = state->textSystem;
    HealthSystem *healthSystem = state->health;


    glm::mat4 cameraMvp;
    for(int i = 0; i < state->entitiesReg->count; i++) {
        if((state->entitiesReg->comp[i] & (COMP_CAMERA))) {
            if(cameraSystem->isActive[i]) {
                cameraMvp = cameraSystem->projection[i] * cameraSystem->view[i];
            }
        }
    }

    if(renderSystems & SYS_RENDER) {
        for(int i = 0; i < state->entitiesReg->count; i++) {
            if((state->entitiesReg->comp[i] & (COMP_RENDER))) {

                glm::vec2 pos = {0,0};
                glm::vec2 rot = {0,1};
                pos = movementSystem->pos[i];
                rot = movementSystem->rot[i];
                int count = renderSystem->vertCount[i];
                Vertex *verts = renderSystem->verts[i];
                //if(i == 5) {
                //    for(int j = 0; j < renderSystem->vertCount[i]; j++) {
                //        Vertex *vert = &verts[j];
                //        vert->color = {1,1,1,0.8};
                //    }
                //}

                if(count == 3) {
                    PushTrianges2(cameraMvp, &memory->transient, verts, count, pos, rot);
                } else {
                    PushLoop2(cameraMvp, &memory->transient, verts, count, pos, rot);
                }
            }

            if((state->entitiesReg->comp[i] & (COMP_TEXT))) {
                glm::vec2 pos = textSystem->pos[i];
                glm::vec4 color = textSystem->color[i];
                Anchor anchor = textSystem->anchor[i];
                FieldType field = textSystem->fieldType[i];
                EntityID source = textSystem->source[i];
                switch (field) {
                    case FIELD_HEALTH:
                        if(state->entitiesReg->comp[source] & COMP_HEALTH) {
                            float hp = healthSystem->currentHP[source];
                            PushTextf(
                                state,
                                &memory->transient,
                                pos,
                                color,
                                anchor,
                                "HEALTH: %i",
                                (int)hp
                            );
                        }
                        break;
                    case FIELD_SPEED:
                        if(state->entitiesReg->comp[i] & COMP_HEALTH) {
                            PushTextf(
                                state,
                                &memory->transient,
                                pos,
                                color,
                                anchor,
                                "HEALTH: %f",
                                100.0f
                            );
                        }
                        break;
                }
            }
        }
    }


    if(renderSystems & SYS_PARTICLES) {
        int vertexCount = 0;
        static ParticleVertex verts[MAX_PARTICLES];
        for(int i = 0; i < state->particles->count; i++) {
            if(!state->particles->active[i]) {
                continue;
            }
            glm::vec2 pos = state->particles->pos[i];
            glm::vec4 color = state->particles->color[i];
            verts[vertexCount] = { pos, color };
            vertexCount++;
        }

        if(vertexCount > 0) {
            PushParticleBatch(cameraMvp, &memory->transient, verts, vertexCount);
        }
    }

    {
        // TODO: Text
        PushTextf(state, &memory->transient, {10, 20}, {1,1,1,1}, TOP_LEFT, "FPS: %f", (1.0f / frame->deltaTime));
        //size_t size_in_bytes = sizeof(memory->permanent.used);
        size_t sizeInBytes = memory->permanent.used;
        double sizeInKB = (sizeInBytes) / 1024.0;
        double sizeInMB = (sizeInKB) / 1024.0;
        PushTextf(state, &memory->transient, {300, 20}, {1,1,1,1}, TOP_RIGHT, "perm size: %.2f", sizeInKB);
    }
    {
        size_t sizeInBytes = memory->transient.used;
        double sizeInKB = (sizeInBytes) / 1024.0;
        double sizeInMB = (sizeInKB) / 1024.0;
        PushTextf(state, &memory->transient, {150, 20}, {1,1,1,1}, TOP_RIGHT, "trans size: %.2f", sizeInKB);
    }
    {
        size_t sizeInBytes = memory->sound.used;
        double sizeInKB = (sizeInBytes) / 1024.0;
        double sizeInMB = (sizeInKB) / 1024.0;
        PushTextf(state, &memory->transient, {150, 20}, {1,1,1,1}, BOTTOM_LEFT, "sound size: %f", sizeInKB);
    }


    state->commands = memory->transient.base;
    state->renderCommandsCount = memory->transient.used;
}

void GameRender(GameState *state, PlatformMemory *memory, PlatformFrame* frame) {
    uint32_t renderSystems = 0;
    SceneStack* stack = &state->sceneStack;

    for (int i = stack->count - 1; i >= 0; i--) {
        Scene* s = stack->scenes[i];
        renderSystems |= s->systemMask;

        if (s->blocksRender) {
            break;
        }
    }

    RenderEntities(state, memory, frame, renderSystems);
}


void GameSound(GameState *state, PlatformMemory *memory) {
    ArenaReset(&memory->sound);
    SoundSystem* sound = state->sound;
    static int test = 0;
    if(test == 0) {
        PushAudioPlayStream(&memory->sound, SOUND_TITLE_MUSIC, 0.2f);
        PushAudioPlayStream(&memory->sound, SOUND_AMBIENT_WIND, 0.2f);
    }
    test = 1;

    for(int i = 0; i < sound->count; i++) {
        // TODO: Use a table
        //printf("Sound Type: %i\n", sound->type[i]);
        if(sound->type[i] == EVENT_ENTITY_ATTACK) {
            PushAudioPlay(&memory->sound, SOUND_LASER, 0.2f);
        }
        if(sound->type[i] == EVENT_ENTITY_DEATH) {
            PushAudioPlay(&memory->sound, SOUND_EXPLOSION, 0.2f);
        }
    }

    sound->count = 0;

    state->soundCommands = memory->sound.base;
    state->soundCommandsCount = memory->sound.used;
}
