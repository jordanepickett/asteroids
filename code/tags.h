#include <cstdint>

typedef uint32_t TagMask;
enum {
    TAG_NONE      = 0,
    TAG_PLAYER  = 1 << 0,
    TAG_ASTEROID    = 1 << 1,
    TAG_WALL = 1 << 2,
    TAG_MISSLE    = 1 << 3,
    TAG_LASER    = 1 << 4,
    TAG_NUKE    = 1 << 5,
    TAG_SHIP    = 1 << 6,
};

