#pragma once
#include "platform.h"
typedef struct GameState GameState;

enum SceneType {
    SCENE_START,
    SCENE_SETTINGS,
    SCENE_GAME,
    SCENE_PAUSE
};

typedef struct {
    SceneType type;
    void (*update)(GameState* state, PlatformFrame* frame, PlatformMemory* memory);
    void (*onEnter)(GameState* state);
    void (*onExit)(GameState* state);
} Scene;
