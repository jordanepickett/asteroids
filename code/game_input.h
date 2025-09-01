#include "platform_input.h"

enum GameAction {
    ACTION_MOVE_UP,
    ACTION_MOVE_DOWN,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,
    ACTION_FIRE,
    ACTION_DASH,
    ACTION_COUNT
};

struct GameInput {
    ButtonState actions[ACTION_COUNT];
    float moveX; // analog axis (keyboard or stick)
    float moveY;
};
