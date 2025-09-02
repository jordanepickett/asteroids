
enum ButtonState1 {
    BUTTON_RELEASED,
    BUTTON_PRESSED,
    BUTTON_HELD
};

struct ControllerState1 {
    ButtonState1 buttons[16];
    float axes[8];
};

struct PlatformInput1 {
    ButtonState1 keys[512];
    int controllerCount;
    ControllerState1 controllers [4];
};

struct ButtonState {
    int halfTransitionCount;
    bool endedDown;
};

struct ControllerInput {
    bool isConnected;
    bool isAnalog;

    float stickAverageX;
    float stickAverageY;

    union {
        ButtonState Buttons[12];
        struct {
            ButtonState moveUp;
            ButtonState moveDown;
            ButtonState moveLeft;
            ButtonState moveRight;

            ButtonState actionUp;
            ButtonState actionDown;
            ButtonState actionLeft;
            ButtonState actionRight;

            ButtonState leftShoulder;
            ButtonState rightShoulder;

            ButtonState back;
            ButtonState start;
        };
    };
};

struct GameInput {
    ControllerInput controllers[5];
};
