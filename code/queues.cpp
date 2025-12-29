#include "game/entities/entities.h"
#include "defs.h"
#include "systems.cpp"
#include "queues.h"
#include "game.h"
#include "systems.h"
#include <cassert>

static void CollisionQueueInit(CollisionQueue *q) { q->count = 0; }

static void ProcessProjectileFire(GameState *state) {
    ProjectileQueue *queue = state->projectile;
    TransformSystem *transformSystem = state->transforms;

    for(int i = 0; i < queue->count; i++) {
        EntityID origin = queue->events[i].origin;
        float lifeTime = queue->events[i].lifeTime;
        if (!state->entitiesReg->active[origin]) continue;

        glm::vec2 originPos = transformSystem->pos[origin];
        glm::vec2 originRot = transformSystem->rot[origin];

        glm::vec2 pos = {
            originPos.x + originRot.x,
            originPos.y + originRot.y,
        };
        EntityID projectile = CreateEntity2(state, pos);

        AddTag(state, projectile, TAG_MISSLE);
        AddMovement(state, projectile, originRot * 30.0f);
        AddDamage(state, projectile, 1.0f, TAG_ASTEROID);
        AddMesh(state, projectile, MISSLE, 4);
        AddLifeTimeSystem(state, projectile, lifeTime);
        AddCollision(state, projectile, 1.0f);
        AddEntityEvent(state, EVENT_ENTITY_ATTACK, origin, originPos, originRot);
    }

    // Reset the queue!!
    queue->count = 0;
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
            EntityID emitter = CreateEntity2(state, { -position.x, position.y });
            AddMovement(state, emitter, direction);
            AddEmitter(
                state,
                emitter,
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
    assert(sound->count < 5);

    switch(type) {
        case EVENT_ENTITY_DEATH: {
            if(state->entitiesReg->comp[entityId] & COMP_FLOATABLE) {
                sound->type[sound->count] = EVENT_ENTITY_DEATH;
                sound->entity[sound->count] = entityId;
                sound->variant[sound->count] = 0;
            }
        } break;
        case EVENT_ENTITY_ATTACK: {
            sound->type[sound->count] = EVENT_ENTITY_ATTACK;
            sound->entity[sound->count] = entityId;
            sound->variant[sound->count] = 0;
        }
        default: {
        } break;
    }
    sound->count++;
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
