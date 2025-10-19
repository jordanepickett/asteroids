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
};

