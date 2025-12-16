
#include "defs.h"
#include "systems.cpp"
#include "queues.h"
#include "game.h"
#include "systems.h"
#include <cassert>
#include <cstdio>

static void AddEvent(GameState* state,
                     EventType eventType,
                     EntityID entityId,
                     glm::vec2 position,
                     glm::vec2 direction) {

    EventQueue* queue = state->events;

    queue->events[queue->count].type = eventType;
    queue->events[queue->count].entityId = entityId;
    queue->events[queue->count].position = position;
    queue->events[queue->count].direction = direction;
    queue->events[queue->count].variant = 0;
    queue->count++;
}

static void CollisionQueueInit(CollisionQueue *q) { q->count = 0; }

static void ProcessProjectileFire(GameState *state) {
    ProjectileQueue *queue = state->projectile;
    MovementSystem *movement = state->movement;

    for(int i = 0; i < queue->count; i++) {
        EntityID origin = queue->events[i].origin;
        float lifeTime = queue->events[i].lifeTime;
        if (!state->entitiesReg->active[origin]) continue;

        glm::vec2 originPos = movement->pos[origin];
        glm::vec2 originRot = movement->rot[origin];

        EntityID projectile = CreateEntity2(state);
        glm::vec2 pos = {
            originPos.x + originRot.x,
            originPos.y + originRot.y,
        };

        AddTag(state, projectile, TAG_MISSLE);
        AddMovement(state, projectile, pos, originRot, originRot * 30.0f);
        AddDamage(state, projectile, 1.0f, TAG_ASTEROID);
        AddRender(state, projectile, MISSLE, 4);
        AddLifeTimeSystem(state, projectile, lifeTime);
        AddCollision(state, projectile, 1.0f);
        AddEvent(state, EVENT_ENTITY_ATTACK, origin, originPos, originRot);
    }

    // Reset the queue!!
    queue->count = 0;
}

inline void CheckAndDeleteEntity(GameState *state, EntityID id) {

    bool isEntityQueued = false;
    for(int i = 0; i < state->entitiesReg->deleteCount; i++) {
        if (id == state->entitiesReg->toDelete[i]) {
            isEntityQueued = true;
        }
    }
    if(!isEntityQueued) {
        printf("Queued To Die: %i\n", id);
        state->entitiesReg->toDelete[state->entitiesReg->deleteCount++] = id;
        if(state->entitiesReg->comp[id] & COMP_MOVEMENT) {
            MovementSystem* system = state->movement;
            AddEvent(state, EVENT_ENTITY_DEATH, id, system->pos[id], { 0, 0 });
        }
    }
}

static void ProcessCollisions(GameState *state) {
    CollisionQueue *queue = state->collisions;
    MovementSystem *movement = state->movement;
    assert(queue->count < 50);

    for(int i = 0; i < queue->count; i++) {
        EntityID a = queue->events[i].a;
        EntityID b = queue->events[i].b;

        if(state->entitiesReg->comp[a] & COMP_DAMAGE) {
            // Check if has health
            if(state->damage->tags[a] & state->entitiesReg->tag[b]) {
                CheckAndDeleteEntity(state, b);
            }
        }

        if(state->entitiesReg->comp[b] & COMP_DAMAGE) {
            // Check if has health
            if(state->damage->tags[b] & state->entitiesReg->tag[a]) {
                CheckAndDeleteEntity(state, a);
            }
        }
    }

    // Reset the queue!!
    queue->count = 0;

}

static void ProcessEmitterEvents(
    GameState *state,
    EventType type,
    EntityID entityId,
    glm::vec2 position,
    glm::vec2 direction
) {
    switch(type) {
     case EVENT_ENTITY_DEATH: {
            EntityID emitter = CreateEntity2(state);
            AddMovement(state, emitter, { -position.x, position.y }, {0, 1}, direction);
            AddEmitter(
                state,
                emitter,
                position,
                {0, 0},
                {5, 5},
                25,
                0,
                0.5,
                {1, 1, 0, 1},
                {1, 0, 0, 1},
                1,
                1
            );
            AddLifeTimeSystem(state, emitter, 0.5f);

        } break;
        default: {
            break;
        }

    }

}

static void AddSoundEvent(GameState* state, EventType type, EntityID entityId) {
    SoundSystem* sound = state->sound;
    if(sound->count > 5) {
        return;
    }
    switch(type) {
        case EVENT_ENTITY_DEATH: {

        } break;
        case EVENT_ENTITY_ATTACK: {
            sound->count++;
            sound->type[sound->count] = EVENT_ENTITY_ATTACK;
            sound->entity[sound->count] = entityId;
            sound->variant[sound->count] = 0;
            default: {
            } break;
        }
    }
}

static void ProcessEvents(GameState *state) {
    EventQueue *queue = state->events;
    assert(queue->count < 50);

    for(int i = 0; i < queue->count; i++) {
        // Call Processors for emitters/sound
        ProcessEmitterEvents(
            state,
            queue->events[i].type,
            queue->events[i].entityId,
            queue->events[i].position,
            queue->events[i].direction
        );

        AddSoundEvent(state, queue->events[i].type, queue->events[i].entityId);
    }

    // Reset the queue!!
    queue->count = 0;


}
