
enum ButtonState {
    BUTTON_RELEASED,
    BUTTON_PRESSED,
    BUTTON_HELD
};

struct ControllerState {
    ButtonState buttons[16];
    float axes[8];
};

struct PlatformInput {
    ButtonState keys[512];
    int controllerCount;
    ControllerState controllers [4];
};
