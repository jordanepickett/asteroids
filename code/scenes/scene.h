#pragma once
#include "platform.h"
#include <cstdint>
typedef struct GameState GameState;

enum SceneType {
    SCENE_START,
    SCENE_SETTINGS,
    SCENE_GAME,
    SCENE_PAUSE
};

typedef struct {
    SceneType type;
    int sceneEntities[20];
    int sceneEntitiesCount;
    uint32_t systemMask;
    bool blocksUpdate;
    bool blocksRender;
    void (*update)(GameState* state, PlatformFrame* frame, PlatformMemory* memory);
    void (*onEnter)(GameState* state);
    void (*onExit)(GameState* state);
} Scene;

void SceneStackPush(GameState* state, Scene* scene);
void SceneStackPop(GameState* state);
Scene* SceneStackTop(GameState* state);
