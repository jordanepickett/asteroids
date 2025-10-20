
#include "systems.cpp"
#include "queues.h"
#include "game.h"
#include <cstdio>

static void CollisionQueueInit(CollisionQueue *q) { q->count = 0; }

static void ProcessProjectileFire(GameState *state) {
    ProjectileQueue *queue = state->projectile;
    MovementSystem *movement = state->movement;

    for(int i = 0; i < queue->count; i++) {
        EntityID origin = queue->events[i].origin;
        float lifeTime = queue->events[i].lifeTime;
        int movementIndex = movement->id_to_index[origin];
        glm::vec2 originPos = movement->pos[movementIndex];
        glm::vec2 originRot = movement->rot[movementIndex];

        EntityID projectile = CreateEntity2(state);
        glm::vec2 pos = {
            originPos.x + originRot.x,
            originPos.y + originRot.y,
        };

        AddMovement(state, projectile, pos, originRot, originRot * 50.0f);
        AddLifeTimeSystem(state, projectile, lifeTime);
    }

    // Reset the queue!!
    queue->count = 0;
}
