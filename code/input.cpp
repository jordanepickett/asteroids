#include "platform.h"

inline static bool WasPressed(const ButtonState& newState) {
    return (newState.halfTransitionCount > 0 && newState.endedDown);
}

inline bool WasReleased(const ButtonState& newState) {
    return (newState.halfTransitionCount > 0 && !newState.endedDown);
}

inline bool IsDown(const ButtonState& newState) {
    return newState.endedDown;
}
