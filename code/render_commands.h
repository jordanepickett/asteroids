#pragma once

#include <cstdint>
#include <glm/glm.hpp>

enum RenderCommandType {
    RENDER_CMD_CLEAR,
    RENDER_CMD_DRAW_TRIANGLES,
    RENDER_CMD_DRAW_TEXT
};

struct RenderCommandHeader {
    RenderCommandType type;
    uint32_t size;
};

struct RenderCommandClear {
    RenderCommandHeader header;
    glm::vec3 color;
};

struct RenderCommandDrawTriangles {
    RenderCommandHeader header;
    glm::mat4 mvp;
    int vertexCount;
    const void* vertices;
};

struct RenderCommandDrawText {
    RenderCommandHeader header;
    glm::vec2 position;
    glm::vec4 color;
    int length;
};
