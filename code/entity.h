#pragma once
#include <glm/glm.hpp>

enum EntityType {
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_ASTEROID,
    ENTITY_WALL,
    ENTITY_MISSLE,
    ENTITY_PARTICLE,
    ENTITY_ALIEN,
};

struct Transform {
    glm::vec2 position;
    glm::vec2 rotation;
    glm::vec2 scale;

    glm::vec2 velocity;
};

struct Vertex {
    glm::vec2 position;
    glm::vec4 color;
};

struct TextVertex {
    glm::vec2 position;
    float u, v;
    glm::vec4 color;
};

struct Entity {
    EntityType type;
    bool isActive;
    Vertex *model;
    Transform transform;
    float lifeTime = 0;
    float radius = 0.5f;
};

