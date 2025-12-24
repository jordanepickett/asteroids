#include "game.h"

void AddEntityEvent(GameState* state,
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

void CheckAndDeleteEntity(GameState *state, EntityID id) {

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
            AddEntityEvent(state, EVENT_ENTITY_DEATH, id, system->pos[id], { 0, 0 });
        }
    }
}

