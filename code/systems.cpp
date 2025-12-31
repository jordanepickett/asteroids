#include "systems.h"
#include "particles.h"
#include "defs.h"
#include "game.h"
#include "input.cpp"
#include "platform.h"
#include "ui/button.h"
#include <glm/gtc/type_ptr.hpp>

static void MovementSystemInit(MovementSystem *m) {
    for (int i = 0; i < MAX_ENTITIES; ++i) m->present[i] = -1;
}

static void LifetimeSystemInit(LifeTimeSystem *l) {
    for (int i = 0; i < MAX_LIFETIMES; ++i) l->present[i] = -1;
}

static void CollisionSystemInit(CollisionSystem *c) {
    for (int i = 0; i < MAX_ENTITIES; ++i) c->present[i] = -1;
}

static void MeshSystemInit(MeshSystem *r) {
    for (int i = 0; i < MAX_ENTITIES; ++i) r->present[i] = -1;
}

static void HealthSystemInit(HealthSystem *h) {
    for (int i = 0; i < MAX_ENTITIES; ++i) h->present[i] = -1;
}

static void DamageSystemInit(DamageSystem *d) {
    for (int i = 0; i < MAX_ENTITIES; ++i) d->present[i] = -1;
}

static void FloatableSystemInit(FloatableSystem *f) {
    for (int i = 0; i < MAX_FLOATABLES; ++i) f->present[i] = -1;
}

static void EntityRegistryInit(EntityRegistry *e) {
    e->count = 0;
    e->freeCount = 0;
    e->deleteCount = 0;
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        e->comp[i] = COMP_NONE;
        e->active[i] = 0;
    }
}

static void AddTransform(GameState *state, EntityID id) {
    TransformSystem *system = state->transforms;

    system->pos[id] = {0, 0};
    system->rot[id] = {0, 1};
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_TRANSFORM;
    
}

static void AddTransform(GameState *state, EntityID id, glm::vec2 pos) {
    TransformSystem *system = state->transforms;

    system->pos[id] = pos;
    system->rot[id] = {0, 1};
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_TRANSFORM;
    
}

static EntityID CreateEntity2(GameState* state) {
    EntityID id;
    if (state->entitiesReg->freeCount > 0) {
        id = state->entitiesReg->freeList[--state->entitiesReg->freeCount];
#ifdef DEBUG
        printf("Old Entity Slot found: %i\n", id);
#endif
    } else {
        assert(state->entitiesReg->count < MAX_ENTITIES);
        id = state->entitiesReg->count++;
        printf("New Entity ID: %i\n", id);
    }

    state->entitiesReg->active[id] = 1;
    state->entitiesReg->comp[id] = COMP_NONE;
    state->entitiesReg->tag[id] = TAG_NONE;
    AddTransform(state, id);
#ifdef DEBUG
    printf("Creating Entity with ID: %i\n", id);
#endif
    return id;
}

static EntityID CreateEntity2(GameState* state, glm::vec2 pos) {
    EntityID id;
    if (state->entitiesReg->freeCount > 0) {
        id = state->entitiesReg->freeList[--state->entitiesReg->freeCount];
#ifdef DEBUG
        printf("Old Entity Slot found: %i\n", id);
#endif
    } else {
        assert(state->entitiesReg->count < MAX_ENTITIES);
        id = state->entitiesReg->count++;
        printf("New Entity ID: %i\n", id);
    }

    state->entitiesReg->active[id] = 1;
    state->entitiesReg->comp[id] = COMP_NONE;
    state->entitiesReg->tag[id] = TAG_NONE;
    AddTransform(state, id, pos);
#ifdef DEBUG
    printf("Creating Entity with ID: %i\n", id);
#endif
    return id;
}

static bool HasTag(GameState* state, EntityID id, TagMask tag) {
    return state->entitiesReg->tag[id] & tag;
}

static void AddTag(GameState* state, EntityID id, TagMask tag) {
    state->entitiesReg->tag[id] |= tag;
}

static void RemoveTag(GameState* state, EntityID id, TagMask tag) {
    state->entitiesReg->tag[id] &= ~tag;
}

static void AddCamera(
    GameState *state,
    EntityID id,
    glm::mat4 lookAt,
    glm::vec3 pos,
    bool isLocked,
    bool isActive
) {
#ifdef DEBUG
    printf("Adding Camera to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    CameraSystem *system = state->cameraSys;

    float left   = -20.0f;
    float right  =  20.0f;
    float bottom = -20.0f;
    float top    =  20.0f;
    float nearZ  = -1.0f;
    float farZ   =  1.0f;

    system->projection[id] = glm::ortho(left, right, bottom, top, nearZ, farZ);
    system->view[id] = lookAt;
    system->isLocked[id] = isLocked;
    system->isActive[id] = isActive;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_CAMERA;
}

static void AddEmitter(
    GameState *state,
    EntityID id,
    glm::vec2 spawnVelocityBase,
    glm::vec2 spawnVelocityVariance,
    float spawnRate,
    float spawnTimer,        // Accumulator
    float particleLifetime,
    glm::vec4 startColor,
    glm::vec4 endColor,
    float startSize,
    float endSize,
    EntityID parentEntity = -1) {
#ifdef DEBUG
    printf("Adding Emitter to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_EMITTERS);
    EmitterSystem *system = state->emitter;

    system->spawnVelocityBase[id] = spawnVelocityBase;
    system->spawnVelocityVariance[id] = spawnVelocityVariance;
    system->spawnRate[id] = spawnRate;
    system->spawnTimer[id] = spawnTimer;        // Accumulator
    system->particleLifetime[id] = particleLifetime;
    system->startColor[id] = startColor;
    system->endColor[id] = endColor;
    system->startSize[id] = startSize;
    system->endSize[id] = endSize;
    if(parentEntity != -1) {
        system->parentEntity[id] = parentEntity;   // Optional entity to follow
    }
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_EMITTER;
}

static void AddButton(
    GameState *state,
    EntityID id,
    glm::vec2 size,
    ButtonBehavior behavior,
    bool isSelectable
) {
#ifdef DEBUG
    printf("Adding UI to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    ButtonSystem *system = state->buttons;
    system->size[id] = size;
    system->behavior[id] = behavior;
    system->isSelectable[id] = isSelectable;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_BUTTON;
}

static void AddText(
    GameState *state,
    EntityID id,
    glm::vec4 color,
    Anchor anchor,
    EntityID source,
    FieldType fieldType
) {
#ifdef DEBUG
    printf("Adding Text to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    TextSystem *system = state->textSystem;
    system->color[id] = color;
    system->anchor[id] = anchor;
    system->source[id] = source;
    system->fieldType[id] = fieldType;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_TEXT;
}

static void AddLight(GameState *state,
                     EntityID id,
                     glm::vec3 color,
                     float radius,
                     float intesity,
                     EntityID source = -1
                     ) {
#ifdef DEBUG
    printf("Adding Light to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    LightSystem *system = state->light;
    system->color[id] = color;
    system->radius[id] = radius;
    system->intesity[id] = intesity;
    system->source[id] = source;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_LIGHT;
}

static void AddCollision(GameState *state, EntityID id, float size) {
#ifdef DEBUG
    printf("Adding Collision to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    CollisionSystem *system = state->collision;
    system->size[id] = size;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_COLLISION;
}

static void AddMesh(GameState *state, EntityID id, Vertex *verts, int vertCount) {
#ifdef DEBUG
    printf("Adding Render to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    MeshSystem *system = state->meshes;
    system->verts[id] = verts;
    system->vertCount[id] = vertCount;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_MESH;
}

static void AddHealth(GameState *state, EntityID id, float hp) {
#ifdef DEBUG
    printf("Adding Health to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    HealthSystem *system = state->health;
    system->currentHP[id] = hp;
    system->maxHP[id] = hp;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_HEALTH;
}

static void AddDamage(GameState *state, EntityID id, float amount, TagMask canHit) {
#ifdef DEBUG
    printf("Adding Damage to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    DamageSystem *system = state->damage;
    system->tags[id] = canHit;
    system->damage[id] = amount;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_DAMAGE;
}

static void AddMovement(
    GameState *state,
    EntityID id,
    glm::vec2 vel
) {
#ifdef DEBUG
    printf("Adding Movement to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    MovementSystem *m = state->movement;
    m->vel[id] = vel;
    m->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_MOVEMENT;
}

static void AddFireMissleSystem(GameState *state, EntityID id) {
#ifdef DEBUG
    printf("Adding Fire Missle to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    FireMissleSystem *system = state->fireMissile;
    //system->offset[idx] = offset;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_FIRE_MISSLE;
}

static void AddLifeTimeSystem(GameState *state, EntityID id, float lifeTime) {
#ifdef DEBUG
    printf("Adding Lifetime to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    LifeTimeSystem *system = state->lifetime;
    system->lifetime[id] = lifeTime;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_LIFETIME;
}

static void AddPlayerInput(GameState *state, EntityID id) {
#ifdef DEBUG
    printf("Adding Player Input to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    PlayerInputSystem *system = state->playerInput;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_PLAYER_INPUT;
}


static void AddFloatable(GameState *state, EntityID id) {
#ifdef DEBUG
    printf("Adding Floatable to: %i\n", id);
#endif
    assert(id >= 0 && id < MAX_ENTITIES);
    FloatableSystem *system = state->floatable;
    //system->offset[idx] = offset;
    system->present[id] = 1;
    state->entitiesReg->comp[id] |= COMP_FLOATABLE;
}

static void MovementUpdate(GameState* state, PlatformFrame *frame) {
    MovementSystem* system = state->movement;
    TransformSystem* transforms = state->transforms;
    for (EntityID i = 0; i < state->entitiesReg->count; ++i) {
        if(!(state->entitiesReg->comp[i] & COMP_MOVEMENT)) {
            continue;
        }

        transforms->pos[i].x += system->vel[i].x * frame->deltaTime;
        transforms->pos[i].y += system->vel[i].y * frame->deltaTime;
    }
}

static glm::vec2 RandomVariance(glm::vec2 variance)
{
    return glm::vec2(
        ((float)rand() / RAND_MAX * 2.f - 1.f) * variance.x,
        ((float)rand() / RAND_MAX * 2.f - 1.f) * variance.y
    );
}

static void EmitterUpdate(GameState* state, float deltaTime) {
    EmitterSystem* system = state->emitter;
    ParticleSystem* particleSystem = state->particles;
    TransformSystem* transformSystem = state->transforms;
    for (EntityID i = 0; i < state->entitiesReg->count; ++i) {
        if(!(state->entitiesReg->comp[i] & COMP_EMITTER)) {
            continue;
        }
        //system->endColor[i] = {1, 0, 0, 1};
        glm::vec2 parentVelocity = { 0, 0 };
        system->spawnTimer[i] += deltaTime;
        float spawnInterval = 1.0f / system->spawnRate[i];

        while (system->spawnTimer[i] >= spawnInterval) {
            system->spawnTimer[i] -= spawnInterval;

            // Add parent velocity to emitted particles
            glm::vec2 vel = system->spawnVelocityBase[i] + 
                parentVelocity +
                RandomVariance(system->spawnVelocityVariance[i]);

            CreateParticle(particleSystem, transformSystem->pos[i], vel,
                           system->particleLifetime[i],
                           system->startColor[i],
                           system->endColor[i]);
        }
    }
}

static void PlayerInputUpdate(
    GameState* state,
    PlatformFrame *frame
) {
    PlayerInputSystem* inputSystem = state->playerInput;
    MovementSystem*  movementSystem = state->movement;
    TransformSystem*  transformSystem  = state->transforms;
    for (int i = 0; i < state->entitiesReg->count; ++i) {
        if(!(state->entitiesReg->comp[i] & COMP_PLAYER_INPUT)) {
            continue;
        }
        float rightBurst = 0;
        float leftBurst = 0;
        float lx = 0;
        float ly = 0;

        if(frame->input.controllers[0].rightShoulder.endedDown) {
            rightBurst = 1;
        } else {
            rightBurst = 0;
        }

        lx = frame->input.controllers[0].stickAverageX;
        ly = frame->input.controllers[0].stickAverageY;

        float acceleration = 5.0f;
        float maxSpeed     = 20.0f;
        float damping      = 0.992f;
        float rotLerp   = 15.0f;

        glm::vec2 input(lx, ly);

        input.y = -input.y;
        input.x = -input.x;

        if(glm::length(input) > 0.0f) {
            transformSystem->rot[i] = glm::normalize(glm::mix(
                transformSystem->rot[i],
                input, 
                rotLerp * frame->deltaTime
            ));
        }

        if(rightBurst) {
            movementSystem->vel[i] += transformSystem->rot[i];
        }

        if (glm::length(movementSystem->vel[i]) > maxSpeed) {
            movementSystem->vel[i] = glm::normalize(
                movementSystem->vel[i]) * maxSpeed;
        }

        movementSystem->vel[i] *= damping;

        //movementSystem->pos[movementIndex] += movementSystem->vel[movementIndex] * frame->deltaTime;
    }
}

static void CollisionUpdate(
    GameState *state,
    CollisionQueue *queue
) {
    CollisionSystem* collisionSystem = state->collision;
    TransformSystem* transformSystem = state->transforms;
    for(int i = 0; i < state->entitiesReg->count; i++) {
        if(!(state->entitiesReg->comp[i] & COMP_COLLISION)) {
            continue;
        }
        for (int j = i+1; j < state->entitiesReg->count; ++j) {
            if(!(state->entitiesReg->comp[j] & COMP_COLLISION)) {
                continue;
            }
            float aX = transformSystem->pos[i].x;
            float bX = transformSystem->pos[j].x;

            float aY = transformSystem->pos[i].y;
            float bY = transformSystem->pos[j].y;

            float dx = aX - bX;
            float dy = aY - bY;

            float dist2 = dx*dx + dy*dy;

            float r = collisionSystem->size[i] + collisionSystem->size[j];

            if(dist2 < r*r) {
                queue->events[queue->count].a = i;
                queue->events[queue->count].b = j;
                queue->count++;
            }
        }
    }
}

static void FireMissleUpdate(
    GameState* state,
    PlatformFrame *frame
) {
    FireMissleSystem *system = state->fireMissile;
    ProjectileQueue *queue = state->projectile;

    for(int i = 0; i < state->entitiesReg->count; i++) {
        if(!(state->entitiesReg->comp[i] & COMP_FIRE_MISSLE)) {
            continue;
        }
        if(WasPressed(frame->input.controllers[0].actionDown)) {
            if (queue->count < (int)(sizeof(queue->events)/sizeof(queue->events[0]))) {
                queue->events[queue->count].origin = i;
                //queue->events[queue->count].offset = system->offset[i];
                queue->events[queue->count].lifeTime = 1.0f;
                queue->count++;
            }
        } 
    }
}

static void LifeTimeUpdate(GameState *state, PlatformFrame *frame) {
    LifeTimeSystem *system = state->lifetime;
    for(int i = 0; i < state->entitiesReg->count; i++) {
        if(!(state->entitiesReg->comp[i] & COMP_LIFETIME)) {
            continue;
        }
        system->lifetime[i] -= 1.0f * frame->deltaTime;
        if (system->lifetime[i] <= 0.0f) {
            //printf("Lifetime Entity Dead: %i\n", entity);
            state->entitiesReg->toDelete[state->entitiesReg->deleteCount++] = i;
        }
    }
}

static void RemoveEntityFromSystems(GameState *state, EntityID id) {

    {
        if (state->entitiesReg->comp[id] & COMP_MOVEMENT) {
            state->movement->present[id] = 0;
        }
    }

    {
        if (state->entitiesReg->comp[id] & COMP_LIFETIME) {
            state->lifetime->present[id] = 0;
        }
    }

    {

        if (state->entitiesReg->comp[id] & COMP_MESH) {
            state->meshes->present[id] = 0;
        }
    }

    {
        if (state->entitiesReg->comp[id] & COMP_COLLISION) {
            state->collision->present[id] = 0;
        }
    }

    {
        if (state->entitiesReg->comp[id] & COMP_FLOATABLE) {
            state->floatable->present[id] = 0;
        }
    }
}

static void CleanupDeadEntities(GameState *state) {
    for(int i = 0; i < state->entitiesReg->deleteCount; i++) {
        EntityID entity = state->entitiesReg->toDelete[i];
#ifdef DEBUG
        printf("Entity to delete from systems: %i\n", entity);
#endif
        if(entity != -1) {

            state->entitiesReg->freeList[state->entitiesReg->freeCount++] = entity;
            state->entitiesReg->toDelete[i] = -1;

            state->entitiesReg->active[entity] = 0;
            state->entitiesReg->comp[entity] = COMP_NONE;
        }
    }
    state->entitiesReg->deleteCount = 0;
}
