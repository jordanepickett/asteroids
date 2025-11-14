
#include "systems.cpp"
#include "queues.h"
#include "game.h"
#include "systems.h"

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
    }
}

static void ProcessCollisions(GameState *state) {
    CollisionQueue *queue = state->collisions;
    MovementSystem *movement = state->movement;

    for(int i = 0; i < queue->count; i++) {
        EntityID a = queue->events[i].a;
        EntityID b = queue->events[i].b;

        if(state->entitiesReg->comp[a] & COMP_DAMAGE) {
            if(state->damage->tags[a] & state->entitiesReg->tag[b]) {
                CheckAndDeleteEntity(state, b);
            }
        }

        if(state->entitiesReg->comp[b] & COMP_DAMAGE) {
            if(state->damage->tags[b] & state->entitiesReg->tag[a]) {
                CheckAndDeleteEntity(state, a);
            }
        }
    }

    // Reset the queue!!
    queue->count = 0;

}
