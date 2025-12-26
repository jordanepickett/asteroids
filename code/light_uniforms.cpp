#include "defs.h"
#include "platform.h"
#include "systems.h"
#include <cstdio>

void LightUniformsInit(Program* program) {
    char buf[64];

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        snprintf(buf, sizeof(buf), "lights[%d].position", i);
        program->lightUniforms[i][LU_POSITION] = glGetUniformLocation(program->program, buf);

        snprintf(buf, sizeof(buf), "lights[%d].color", i);
        program->lightUniforms[i][LU_COLOR] = glGetUniformLocation(program->program, buf);

        snprintf(buf, sizeof(buf), "lights[%d].radius", i);
        program->lightUniforms[i][LU_RADIUS] = glGetUniformLocation(program->program, buf);

        snprintf(buf, sizeof(buf), "lights[%d].intensity", i);
        program->lightUniforms[i][LU_INTENSITY] = glGetUniformLocation(program->program, buf);
    }
}

void AddLightsFrame(Program* program, LightSystem* lightSystem, TransformSystem* transformSystem) {
    int lightCount = ArrayCount(lightSystem->color);
    glUniform3f(program->uniformLocations[U_AMBIENT], 0.5f, 0.8f, 0.9f);
    //TODO Dynamic count
    glUniform1i(program->uniformLocations[U_LIGHTCOUNT], 2);
    int lightsInUse = 0;
    for (int i = 0; i < lightCount && i < 16; i++) {
        char uniformName[64];
        if(!lightSystem->present[i]) {
            continue;
        }

        glUniform3f(program->lightUniforms[lightsInUse][LU_POSITION], -transformSystem->pos[i].x, transformSystem->pos[i].y, 1);
        //printf("lights[%d].position location: %f\n", lightsInUse, system->pos[i].x);

        glUniform3f(program->lightUniforms[lightsInUse][LU_COLOR], lightSystem->color[i].r, lightSystem->color[i].g, lightSystem->color[i].b);

        glUniform1f(program->lightUniforms[lightsInUse][LU_RADIUS], lightSystem->radius[i]);

        glUniform1f(program->lightUniforms[lightsInUse][LU_INTENSITY], lightSystem->intesity[i]);
        ++lightsInUse;
    }

    lightsInUse = 0;
}
