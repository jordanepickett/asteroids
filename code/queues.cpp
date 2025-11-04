
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
            originPos.y + originRot.y + 5.0f,
        };

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
        bool aIsAsteroid = state->entitiesReg->comp[a] & COMP_ASTEROID;
        bool bIsAsteroid = state->entitiesReg->comp[b] & COMP_ASTEROID;
        if(aIsAsteroid && bIsAsteroid) {
            continue;
        }
        state->entitiesReg->toDelete[state->entitiesReg->deleteCount++] = a;
        state->entitiesReg->toDelete[state->entitiesReg->deleteCount++] = b;
        printf("A: %i\n", a);
        printf("B: %i\n", b);
    }

    // Reset the queue!!
    queue->count = 0;

}
