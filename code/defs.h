#pragma once

#include <cstdint>

typedef int32_t EntityID;
#define INVALID_ENTITY_ID (-1)
#define MAX_ENTITIES 64
#define MAX_FLOATABLES 15
#define MAX_LIFETIMES 15

enum FieldType {
    FIELD_HEALTH,
    FIELD_SPEED,
};
