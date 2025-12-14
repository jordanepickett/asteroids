#pragma once

#include "defs.h"
#include "glm/ext/vector_float2.hpp"

typedef struct {
    struct { EntityID a, b; } events[50];
    int count;
} CollisionQueue;

typedef struct {
    struct { 
        EntityID origin; 
        glm::vec2 offset;
        float lifeTime;
    } events[20];
    int count;
} ProjectileQueue;

enum EventType {
    EVENT_ENTITY_SPAWN,
    EVENT_ENTITY_DEATH,
    EVENT_ENTITY_HIT,
    EVENT_ENTITY_ATTACK
};

typedef struct {
    struct {
        EventType type;
        EntityID entityId;
        glm::vec2 position;
        glm::vec2 direction;
        uint8_t variant;
    } events[20];
    int count;
} EventQueue;
