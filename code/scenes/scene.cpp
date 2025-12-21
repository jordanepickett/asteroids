#include "scenes/scene.h"
#include "defs.h"
#include "game.h"

void SceneStackPush(GameState *state, Scene *scene) {
    SceneStack* stack = &state->sceneStack;

    if (stack->count >= MAX_SCENES)
        return;

    if (scene->onEnter) {
        scene->onEnter(state);
    }

    stack->scenes[stack->count++] = scene;
}

void SceneStackPop(GameState *state) {
    SceneStack* stack = &state->sceneStack;

    if (stack->count == 0) {
        return;
    }

    Scene* top = stack->scenes[--stack->count];

    if (top->onExit) {
        top->onExit(state);
    }
}

Scene* SceneStackTop(GameState *state) {
    if (state->sceneStack.count == 0) {
        return NULL;
    }

    return state->sceneStack.scenes[state->sceneStack.count - 1];
}
