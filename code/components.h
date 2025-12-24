#include <cstdint>

typedef uint32_t CompMask;
enum {
    COMP_NONE      = 0,
    COMP_MOVEMENT  = 1 << 0,
    COMP_RENDER    = 1 << 1,
    COMP_COLLISION = 1 << 2,
    COMP_HEALTH    = 1 << 3,
    COMP_DAMAGE    = 1 << 4,
    COMP_CAMERA    = 1 << 5,
    COMP_PLAYER_INPUT = 1 << 6,
    COMP_FIRE_MISSLE = 1 << 7,
    COMP_LIFETIME = 1 << 8,
    COMP_FLOATABLE = 1 << 9,
    COMP_TEXT = 1 << 10,
    COMP_LIGHT = 1 << 11,
    COMP_EMITTER = 1 << 12,
    COMP_BUTTON = 1 << 13,
};

