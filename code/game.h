#pragma once
#include "entity.h"
#include "platform.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

struct Camera {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 position;
    bool isLocked;
};

struct GameState {
    GLuint vao;

    void* commands;
    int renderCommandsCount;
    Entity *entities;
    int *freeEntitiesList;
    int freeEntityCount;
    Camera camera;
};

void GameInit(GameState *state, PlatformAPI *platform, PlatformMemory *memory);
void GameUpdate(GameState *state, PlatformFrame *frame, PlatformMemory *memory);
void GameRender(GameState *state, PlatformMemory *memory);
Entity* CreateEntity(GameState *state, EntityType entity);
void DestoryEntity(Entity* entity);
void UpdateEntities(GameState* state, PlatformFrame *frame);
void PlayerInput(GameState* state, PlatformFrame *frame, Entity* entity);
