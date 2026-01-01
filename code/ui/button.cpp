#include "ui/button.h"
#include "defs.h"
#include "scenes/scene.h"
#include "scenes/scene_game.h"
#include "game.h"
#include "scenes/scene_start.h"

void ButtonPressed(GameState *state, ButtonBehavior behavior) {
    switch(behavior) {
        case BUTTON_QUIT: {
            state->isRunning = false;
        } break;
        case BUTTON_PAUSE: {
            SceneStackPop(state);
            SceneStackPush(state, &SceneGame);
        } break;
        case BUTTON_MAIN_MENU: {
            SceneStackClear(state, &SceneStart);
        } break;
        case BUTTON_START_GAME: {
            SceneStackPop(state);
            SceneStackPush(state, &SceneGame);
        } break;
        case BUTTON_SCENE_POP: {
            SceneStackPop(state);
        } break;
        default: {

        } break;
    }
}
