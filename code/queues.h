#pragma once

#include "defs.h"

typedef struct {
    // collision events are pairs of entity IDs
    struct { EntityID a, b; } events[256];
    int count;
} CollisionQueue;

static void CollisionQueueInit(CollisionQueue *q); 
