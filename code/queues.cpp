
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

        int movementIndex = movement->id_to_index[origin];
        glm::vec2 originPos = movement->pos[movementIndex];
        glm::vec2 originRot = movement->rot[movementIndex];

        EntityID projectile = CreateEntity2(state);
        glm::vec2 pos = {
            originPos.x + originRot.x,
            originPos.y + originRot.y,
        };

        AddTag(state, projectile, TAG_MISSLE);
        AddMovement(state, projectile, pos, originRot, originRot * 30.0f);
        AddRender(state, projectile, MISSLE, 4);
        AddLifeTimeSystem(state, projectile, lifeTime);
        AddCollision(state, projectile, 1.0f);
    }

    // Reset the queue!!
    queue->count = 0;
}

static void ProcessCollisions(GameState *state) {
    CollisionQueue *queue = state->collisions;
    MovementSystem *movement = state->movement;

    for(int i = 0; i < queue->count; i++) {
        EntityID a = queue->events[i].a;
        EntityID b = queue->events[i].b;

        //TODO remove all this
        bool aIsAsteroid = state->entitiesReg->comp[a] & COMP_FLOATABLE;
        bool bIsAsteroid = state->entitiesReg->comp[b] & COMP_FLOATABLE;
        if(aIsAsteroid && bIsAsteroid) {
            continue;
        }
        if(HasTag(state, a, TAG_PLAYER) || HasTag(state, b, TAG_PLAYER)) {
            continue;
        }
        if(HasTag(state, a, TAG_MISSLE) && HasTag(state, b, TAG_MISSLE)) {
            continue;
        }

        //TODO Fix this
        // We can get into a state where a potentially collided with 2 things
        // which means they will be in the toDelete list twice and causing the delete
        // count to increment twice for 1 entitiy.
        bool isEntityAQueued = false;
        bool isEntityBQueued = false;
        for(int i = 0; i < state->entitiesReg->deleteCount; i++) {
            if (a == state->entitiesReg->toDelete[i]) {
                isEntityAQueued = true;
            }
            if (b == state->entitiesReg->toDelete[i]) {
                isEntityBQueued = true;
            }
        }
        if(!isEntityAQueued) {
            printf("A: %i\n", a);
            state->entitiesReg->toDelete[state->entitiesReg->deleteCount++] = a;
        }
        if(!isEntityBQueued) {
            printf("B: %i\n", b);
            state->entitiesReg->toDelete[state->entitiesReg->deleteCount++] = b;
        }
    }

    // Reset the queue!!
    queue->count = 0;

}
