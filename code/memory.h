#pragma once

#include <cstdint>

struct MemoryArena {
    uint8_t* base;
    size_t size;
    size_t used;
};

inline void ArenaInit(MemoryArena* arena, void* memory, size_t size) {
    arena->base = (uint8_t*)memory;
    arena->size = size;
    arena->used = 0;
}

inline void* ArenaAlloc(MemoryArena* arena, size_t size) {
    if(arena->used + size <= arena->size) {
        void* ptr = arena->base + arena->used;
        arena->used += size;
        return ptr;
    }

    return nullptr;
}

inline void ArenaReset(MemoryArena* arena) {
    arena->used = 0;
}
