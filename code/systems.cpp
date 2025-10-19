#include "systems.h"
#include "game.h"
#include "platform.h"
#include <cstdio>

static void MovementSystemInit(MovementSystem *system) {
    system->count = 0;
    for (int i = 0; i < MAX_ENTITIES; ++i) system->id_to_index[i] = -1;
}

static void HealthSystemInit(HealthSystem *system) {
    system->count = 0;
    for (int i = 0; i < MAX_ENTITIES; ++i) system->id_to_index[i] = -1;
}

static void DamageSystemInit(DamageSystem *system) {
    system->count = 0;
    for (int i = 0; i < MAX_ENTITIES; ++i) system->id_to_index[i] = -1;
}

static void EntityRegistryInit(EntityRegistry *e) {
    e->count = 0;
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        e->comp[i] = COMP_NONE;
        e->active[i] = 0;
    }
}

static EntityID CreateEntity2(GameState* state) {
    EntityID id = state->entitiesReg->count++;
    assert(id < MAX_ENTITIES);
    state->entitiesReg->active[id] = 1;
    state->entitiesReg->comp[id] = COMP_NONE;
    return id;
}

static void AddMovement(
    GameState *state,
    EntityID id,
    glm::vec2 pos,
    glm::vec2 rot,
    glm::vec2 vel
) {
    MovementSystem *m = state->movement;
    int idx = m->count++;
    m->ids[idx] = id;
    m->pos[idx] = pos;
    m->rot[idx] = rot;
    m->vel[idx] = vel;
    assert(id >= 0 && id < MAX_ENTITIES);
    m->id_to_index[id] = idx;
    state->entitiesReg->comp[id] |= COMP_MOVEMENT;
}

static void AddCollision(GameState *state, EntityID id) {
    state->entitiesReg->comp[id] |= COMP_COLLISION;
}

static void AddPlayerInput(GameState *state, EntityID id) {
    PlayerInputSystem *system = state->playerInput;
    int idx = system->count++;
    system->ids[idx] = id;
    assert(id >= 0 && id < MAX_ENTITIES);
    system->id_to_index[id] = idx;
    state->entitiesReg->comp[id] |= COMP_PLAYER_INPUT;
}

static void PlayerInputUpdate(
    PlayerInputSystem *inputSystem,
    MovementSystem *movementSystem,
    PlatformFrame *frame
) {
    for (int i = 0; i < inputSystem->count; ++i) {
        int entityId = inputSystem->ids[i];
        int movementIndex = movementSystem->id_to_index[entityId];
        float rightBurst = 0;
        float leftBurst = 0;
        float lx = 0;
        float ly = 0;

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
            movementSystem->rot[movementIndex] = glm::normalize(glm::mix(
                movementSystem->rot[movementIndex],
                input, 
                rotLerp * frame->deltaTime
            ));
        }

        if(rightBurst) {
            movementSystem->vel[movementIndex] += movementSystem->rot[movementIndex];
        }

        if (glm::length(movementSystem->vel[movementIndex]) > maxSpeed) {
            movementSystem->vel[movementIndex] = glm::normalize(
                movementSystem->vel[movementIndex]) * maxSpeed;
        }

        movementSystem->vel[movementIndex] *= damping;

        movementSystem->pos[movementIndex] += movementSystem->vel[movementIndex] * frame->deltaTime;
    }
}
