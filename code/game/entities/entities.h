#pragma once
#include "game.h"

void AddEntityEvent(GameState* state,
                    EventType eventType,
                    EntityID entityId,
                    glm::vec2 position,
                    glm::vec2 direction);
void CheckAndDeleteEntity(GameState *state, EntityID id);
