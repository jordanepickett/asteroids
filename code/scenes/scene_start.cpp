#include <stdio.h>
#include "defs.h"
#include "queues.cpp"
#include "scene_start.h"
#include "game.h"
#include "scene_game.h"
#include "platform.h"
#include "scenes/scene.h"
#include "systems.h"
#include "ui/button.h"

// Forward declarations (file-local)
static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory);
static void onEnter(GameState* state);
static void onExit(GameState* state);

// Static scene definition
Scene SceneStart = {
    SCENE_START,
    {},
    0,
    SYS_INPUT | SYS_UI | SYS_RENDER,
    true,
    true,
    update,
    onEnter,
    onExit,
};

// ---- Implementation ----

static void onEnter(GameState* state) {
    printf("[Start] Enter\n");
    ClearGameSystems(state);
    EntityID camera = CreateEntity2(state);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::mat4 lookAt = glm::lookAt(
        cameraPos,
        glm::vec3(0.0f, 0.0f, 0.0f),        // target
        glm::vec3(0.0f, 1.0f, 0.0f)         // up
    );
    AddCamera(state, camera, lookAt, cameraPos, true, true);

    // Utilize anchors for raw position with the transform as the offset from that (For buttons -> children (text))
    EntityID start = CreateEntity2(state, {0, -100});
    EntityID settings = CreateEntity2(state, {0, -50});
    EntityID exit = CreateEntity2(state, {0, 0});

    TextSource startText = { start, "Start Game" };
    ButtonRelationship startRelationship = { exit, settings };
    AddButton(state, start, { 150, 30}, BUTTON_START_GAME, startRelationship, true);
    AddText(
        state,
        start,
        {1,1,1,1},
        CENTER,
        start,
        startText,
        FIELD_SOURCE_LITERAL
    );
    TextSource settingsText = { settings, "Settings" };
    ButtonRelationship settingsRelationship = { start, exit };
    AddButton(state, settings, { 150, 30 }, BUTTON_QUIT, settingsRelationship, true);
    AddText(
        state,
        settings,
        {1,1,1,1},
        CENTER,
        settings,
        settingsText,
        FIELD_SOURCE_LITERAL
    );

    TextSource exitText = { settings, "Exit" };
    ButtonRelationship exitRelationship = { settings, start };
    AddButton(state, exit, { 150, 30 }, BUTTON_QUIT, exitRelationship, true);
    AddText(
        state,
        exit,
        {1,1,1,1},
        CENTER,
        exit,
        exitText,
        FIELD_SOURCE_LITERAL
    );

    state->buttons->selectedButton = start;
}

static void onExit(GameState* state) {
    ClearGameSystems(state);
    printf("[Start] Exit\n");
}

static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory) {
    if(WasPressed(frame->input.controllers[0].moveDown)) {
        state->buttons->selectedButton = state->buttons->relationship[state->buttons->selectedButton].next;
    }

    if(WasPressed(frame->input.controllers[0].moveUp)) {
        state->buttons->selectedButton = state->buttons->relationship[state->buttons->selectedButton].previous;
    }

    if(WasPressed(frame->input.controllers[0].actionDown)) {
        ButtonPressed(state, state->buttons->behavior[state->buttons->selectedButton]);
    }
}
