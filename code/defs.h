#pragma once

#include <cstdint>

typedef int32_t EntityID;
// Scenes
#define MAX_SCENES 4

// Entities
#define INVALID_ENTITY_ID (-1)
#define MAX_ENTITIES 64


// Components
#define MAX_FLOATABLES 15
#define MAX_EMITTERS 64
#define MAX_PARTICLES 2000
#define MAX_LIFETIMES 15


// Lights
#define MAX_LIGHTS 16
#define LIGHT_UNIFORM_COUNT 4

enum FieldType {
    FIELD_HEALTH,
    FIELD_SPEED,
};

typedef enum {
    U_MVP,
    U_MODEL,
    U_NORMAL_MATRIX,
    U_COLOR,
    U_AMBIENT,
    U_LIGHTCOUNT,
    U_COUNT
} UniformID;

struct UniformDef {
    UniformID id;
    const char* name;
};

static constexpr UniformDef UNIFORM_TABLE[] = {
    { U_MVP,          "MVP" },
    { U_MODEL,        "Model" },
    { U_NORMAL_MATRIX,"NormalMatrix" },
    { U_COLOR,        "Color" },
    { U_AMBIENT,        "ambientColor" },
    { U_LIGHTCOUNT,        "lightCount" },
};

enum LightUniform {
    LU_POSITION,
    LU_COLOR,
    LU_RADIUS,
    LU_INTENSITY,
};
