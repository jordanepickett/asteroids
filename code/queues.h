#pragma once

#include "defs.h"
#include "glm/ext/vector_float2.hpp"

typedef struct {
    struct { EntityID a, b; } events[256];
    int count;
} CollisionQueue;

typedef struct {
    struct { 
        EntityID origin; 
        glm::vec2 offset;
        float lifeTime;
    } events[256];
    int count;
} ProjectileQueue;

static void CollisionQueueInit(CollisionQueue *q); 
