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
        printf("%i\n", program->lightUniforms[i][LU_POSITION]);

        snprintf(buf, sizeof(buf), "lights[%d].color", i);
        program->lightUniforms[i][LU_COLOR] = glGetUniformLocation(program->program, buf);

        snprintf(buf, sizeof(buf), "lights[%d].radius", i);
        program->lightUniforms[i][LU_RADIUS] = glGetUniformLocation(program->program, buf);

        snprintf(buf, sizeof(buf), "lights[%d].intensity", i);
        program->lightUniforms[i][LU_INTENSITY] = glGetUniformLocation(program->program, buf);
    }
}

void AddLightsFrame(Program* program, LightSystem* system) {
    int lightCount = ArrayCount(system->pos);
    glUniform3f(program->uniformLocations[U_AMBIENT], 0.5f, 0.8f, 0.9f);
    //TODO Dynamic count
    glUniform1i(program->uniformLocations[U_LIGHTCOUNT], 1);
    int lightsInUse = 0;
    for (int i = 0; i < lightCount && i < 16; i++) {
        char uniformName[64];
        if(!system->present[i]) {
            continue;
        }

        glUniform3f(program->lightUniforms[lightsInUse][LU_POSITION], -system->pos[i].x, system->pos[i].y, 1);
        //printf("lights[%d].position location: %f\n", lightsInUse, system->pos[i].x);

        glUniform3f(program->lightUniforms[lightsInUse][LU_COLOR], system->color[i].r, system->color[i].g, system->color[i].b);

        glUniform1f(program->lightUniforms[lightsInUse][LU_RADIUS], system->radius[i]);

        glUniform1f(program->lightUniforms[lightsInUse][LU_INTENSITY], system->intesity[i]);
        ++lightsInUse;
    }

    lightsInUse = 0;
}
