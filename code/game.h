#include "platform.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

struct Camera {
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 position;
    bool isLocked;
};

struct Player {
    glm::vec2 pos;
    glm::vec2 rotation;
    glm::vec2 velocity;
};

struct GameState {
    GLuint vao;

    void* commands;
    int renderCommandsCount;
    Player player;
    Camera camera;
};

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

void GameInit(GameState *state, PlatformAPI *platform, PlatformMemory *memory);
void GameUpdate(GameState *state, PlatformFrame *frame, PlatformMemory *memory);
void GameRender(GameState *state, PlatformMemory *memory);
