#include <stdio.h>
#include "defs.h"
#include "game.h"
#include "queues.cpp"
#include "platform.h"
#include "scenes/scene.h"
#include "systems.h"

// Forward declarations (file-local)
static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory);
static void onEnter(GameState* state);
static void onExit(GameState* state);

// Static scene definition
Scene ScenePause = {
    SCENE_PAUSE,
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
    printf("[Pause] Enter\n");
    EntityID resume = CreateEntity2(state, {0, -50});
    EntityID start = CreateEntity2(state, {0, 0});
    TextSource resumeText = { resume, "Resume" };
    ButtonRelationship resumeRelationship = { start, start };
    AddButton(state, resume, { 150, 30 }, BUTTON_SCENE_POP, resumeRelationship, false);
    AddText(
        state,
        resume,
        {1,1,1,1},
        CENTER,
        resume,
        resumeText,
        FIELD_SOURCE_LITERAL
    );
    TextSource startText = { start, "Main Menu" };
    ButtonRelationship startRelationship = { resume, resume };
    AddButton(state, start, { 150, 30 }, BUTTON_MAIN_MENU, startRelationship, false);
    AddText(
        state,
        start,
        {1,1,1,1},
        CENTER,
        start,
        startText,
        FIELD_SOURCE_LITERAL
    );

    state->buttons->selectedButton = resume;
    printf("resume: %i\n", resume);
    printf("start: %i\n", start);

    ScenePause.sceneEntities[ScenePause.sceneEntitiesCount++] = resume;
    ScenePause.sceneEntities[ScenePause.sceneEntitiesCount++] = start;
}

static void onExit(GameState* state) {
    printf("[Pause] Exit\n");
    for(int i = 0; i < ScenePause.sceneEntitiesCount; i++) {
        printf("Delete entity: %i\n", ScenePause.sceneEntities[i]);
        CheckAndDeleteEntity(state, ScenePause.sceneEntities[i]);
    }
    ScenePause.sceneEntitiesCount = 0;
}

static void update(GameState* state, PlatformFrame* frame, PlatformMemory* memory) {
    if (WasPressed(frame->input.controllers[0].start)) {
        printf("[Pause] back scene.\n");
        SceneStackPop(state);
    }

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
