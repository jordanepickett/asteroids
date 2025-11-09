#include "systems.h"
#include "defs.h"
#include "game.h"
#include "input.cpp"
#include "platform.h"

static void MovementSystemInit(MovementSystem *m) {
    m->count = 0;
    for (int i = 0; i < MAX_ENTITIES; ++i) m->id_to_index[i] = -1;
}

static void LifetimeSystemInit(LifeTimeSystem *l) {
    l->count = 0;
    for (int i = 0; i < 10; ++i) l->id_to_index[i] = -1;
}

static void CollisionSystemInit(CollisionSystem *c) {
    c->count = 0;
    for (int i = 0; i < MAX_ENTITIES; ++i) c->id_to_index[i] = -1;
}

static void RenderSystemInit(RenderSystem *r) {
    r->count = 0;
    for (int i = 0; i < MAX_ENTITIES; ++i) r->id_to_index[i] = -1;
}

static void HealthSystemInit(HealthSystem *h) {
    h->count = 0;
    for (int i = 0; i < MAX_ENTITIES; ++i) h->id_to_index[i] = -1;
}

static void DamageSystemInit(DamageSystem *d) {
    d->count = 0;
    for (int i = 0; i < MAX_ENTITIES; ++i) d->id_to_index[i] = -1;
}

static void FloatableSystemInit(FloatableSystem *f) {
    f->count = 0;
    for (int i = 0; i < 10; ++i) f->id_to_index[i] = -1;
}

static void EntityRegistryInit(EntityRegistry *e) {
    e->count = 0;
    e->freeCount = 0;
    e->deleteCount = 0;
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        e->comp[i] = COMP_NONE;
        e->active[i] = 0;
        e->toDelete[i] = -1;
    }
}

static EntityID CreateEntity2(GameState* state) {
    EntityID id;
    if (state->entitiesReg->freeCount > 0) {
        id = state->entitiesReg->freeList[--state->entitiesReg->freeCount];
        printf("Old Entity Slot found: %i\n", id);
    } else {
        assert(state->entitiesReg->count < MAX_ENTITIES);
        id = state->entitiesReg->count++;
        printf("New Entity ID: %i\n", id);
    }

    state->entitiesReg->active[id] = 1;
    state->entitiesReg->comp[id] = COMP_NONE;
    state->entitiesReg->tag[id] = TAG_NONE;
    printf("Creating Entity with ID: %i\n", id);
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

static void AddCollision(GameState *state, EntityID id, float size) {
    printf("Adding Collision to: %i\n", id);
    CollisionSystem *system = state->collision;
    int idx = system->count++;
    printf("Collision System Index: %i\n", idx);
    system->ids[idx] = id;
    system->size[idx] = size;
    assert(id >= 0 && id < MAX_ENTITIES);
    system->id_to_index[id] = idx;
    state->entitiesReg->comp[id] |= COMP_COLLISION;
}

static void AddRender(GameState *state, EntityID id, Vertex *verts, int vertCount) {
    printf("Adding Render to: %i\n", id);
    RenderSystem *system = state->render;
    int idx = system->count++;
    printf("Render System Index: %i\n", idx);
    system->ids[idx] = id;
    system->verts[idx] = verts;
    system->vertCount[idx] = vertCount;
    assert(id >= 0 && id < MAX_ENTITIES);
    system->id_to_index[id] = idx;
    state->entitiesReg->comp[id] |= COMP_RENDER;
}

static void AddMovement(
    GameState *state,
    EntityID id,
    glm::vec2 pos,
    glm::vec2 rot,
    glm::vec2 vel
) {
    printf("Adding Movement to: %i\n", id);
    MovementSystem *m = state->movement;
    int idx = m->count++;
    printf("Movement Index: %i\n", idx);
    m->ids[idx] = id;
    m->pos[idx] = pos;
    m->rot[idx] = rot;
    m->vel[idx] = vel;
    assert(id >= 0 && id < MAX_ENTITIES);
    m->id_to_index[id] = idx;
    state->entitiesReg->comp[id] |= COMP_MOVEMENT;
}

static void AddFireMissleSystem(GameState *state, EntityID id) {
    printf("Adding Fire Missle to: %i\n", id);
    FireMissleSystem *system = state->fireMissile;
    int idx = system->count++;
    printf("Fire Missle Index: %i\n", idx);
    system->ids[idx] = id;
    assert(id >= 0 && id < MAX_ENTITIES);
    //system->offset[idx] = offset;
    system->id_to_index[id] = idx;
    state->entitiesReg->comp[id] |= COMP_FIRE_MISSLE;
}

static void AddLifeTimeSystem(GameState *state, EntityID id, float lifeTime) {
    printf("Adding Lifetime to: %i\n", id);
    LifeTimeSystem *system = state->lifetime;
    int idx = system->count++;
    printf("Lifetime Index: %i\n", idx);
    system->ids[idx] = id;
    assert(id >= 0 && id < MAX_ENTITIES);
    system->lifetime[idx] = lifeTime;
    system->id_to_index[id] = idx;
    state->entitiesReg->comp[id] |= COMP_LIFETIME;
}

static void AddPlayerInput(GameState *state, EntityID id) {
    printf("Adding Player Input to: %i\n", id);
    PlayerInputSystem *system = state->playerInput;
    int idx = system->count++;
    printf("Player Input Index: %i\n", idx);
    system->ids[idx] = id;
    assert(id >= 0 && id < MAX_ENTITIES);
    system->id_to_index[id] = idx;
    state->entitiesReg->comp[id] |= COMP_PLAYER_INPUT;
}

static void AddFloatable(GameState *state, EntityID id) {
    printf("Adding Floatable to: %i\n", id);
    FloatableSystem *system = state->floatable;
    int idx = system->count++;
    printf("Floatable Index: %i\n", idx);
    system->ids[idx] = id;
    assert(id >= 0 && id < MAX_ENTITIES);
    //system->offset[idx] = offset;
    system->id_to_index[id] = idx;
    state->entitiesReg->comp[id] |= COMP_FLOATABLE;
}

static void MovementUpdate(MovementSystem *system, PlatformFrame *frame) {
    for (int i = 0; i < system->count; ++i) {
        system->pos[i].x += system->vel[i].x * frame->deltaTime;
        system->pos[i].y += system->vel[i].y * frame->deltaTime;
    }
}

static void PlayerInputUpdate(
    PlayerInputSystem *inputSystem,
    MovementSystem *movementSystem,
    PlatformFrame *frame
) {
    for (int i = 0; i < inputSystem->count; ++i) {
        int entityId = inputSystem->ids[i];
        int movementIndex = movementSystem->id_to_index[entityId];
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
            movementSystem->rot[movementIndex] = glm::normalize(glm::mix(
                movementSystem->rot[movementIndex],
                input, 
                rotLerp * frame->deltaTime
            ));
        }

        if(rightBurst) {
            movementSystem->vel[movementIndex] += movementSystem->rot[movementIndex];
        }

        if (glm::length(movementSystem->vel[movementIndex]) > maxSpeed) {
            movementSystem->vel[movementIndex] = glm::normalize(
                movementSystem->vel[movementIndex]) * maxSpeed;
        }

        movementSystem->vel[movementIndex] *= damping;

        //movementSystem->pos[movementIndex] += movementSystem->vel[movementIndex] * frame->deltaTime;
    }
}

static void CollisionUpdate(
    CollisionSystem *collisionSystem,
    MovementSystem *movementSystem,
    CollisionQueue *queue
) {
    for(int i = 0; i < collisionSystem->count; i++) {
        EntityID entity = collisionSystem->ids[i];
        if (entity == -1) {
            continue;
        }
        //printf("index: %i, entity: %i\n", i, entity);
        int movementIndex = movementSystem->id_to_index[entity];
        for (int j = i+1; j < collisionSystem->count; ++j) {
            if(entity == collisionSystem->ids[j]) {
                continue;
            }
            EntityID entity2 = collisionSystem->ids[j];
            int movementIndex2 = movementSystem->id_to_index[entity2];
            float aX = movementSystem->pos[movementIndex].x;
            float bX = movementSystem->pos[movementIndex2].x;

            float aY = movementSystem->pos[movementIndex].y;
            float bY = movementSystem->pos[movementIndex2].y;

            float dx = aX - bX;
            float dy = aY - bY;

            float dist2 = dx*dx + dy*dy;

            float r = collisionSystem->size[i] + collisionSystem->size[j];

            if(dist2 < r*r) {
                queue->events[queue->count].a = entity;
                queue->events[queue->count].b = collisionSystem->ids[j];
                queue->count++;
            }
        }
    }
}

static void FireMissleUpdate(
    FireMissleSystem *system,
    PlatformFrame *frame,
    ProjectileQueue *queue
) {
    for(int i = 0; i < system->count; i++) {
        if(WasPressed(frame->input.controllers[0].actionDown)) {
            EntityID origin = system->ids[i];
            if (queue->count < (int)(sizeof(queue->events)/sizeof(queue->events[0]))) {
                queue->events[queue->count].origin = origin;
                //queue->events[queue->count].offset = system->offset[i];
                queue->events[queue->count].lifeTime = 1.0f;
                queue->count++;
            }
        } 
    }
}

static void LifeTimeUpdate(GameState *state, PlatformFrame *frame) {
    LifeTimeSystem *system = state->lifetime;
    for(int i = 0; i < system->count; i++) {
        system->lifetime[i] -= 1.0f * frame->deltaTime;
        if (system->lifetime[i] <= 0.0f) {
            EntityID entity = system->ids[i];
            printf("DEAD Index: %i, Entity: %i\n", i, entity);
            //printf("Lifetime Entity Dead: %i\n", entity);
            state->entitiesReg->toDelete[state->entitiesReg->deleteCount++] = entity;
        }
    }
}

static void RemoveEntityFromSystems(GameState *state, EntityID id) {

    {
        if (state->entitiesReg->comp[id] & COMP_MOVEMENT) {
            int idx = state->movement->id_to_index[id];
            int last = --state->movement->count;
            if (idx != last) {
                state->movement->ids[idx]   = state->movement->ids[last];
                state->movement->pos[idx]   = state->movement->pos[last];
                state->movement->rot[idx]   = state->movement->rot[last];
                state->movement->vel[idx]   = state->movement->vel[last];

                state->movement->id_to_index[state->movement->ids[idx]] = idx;
            }
            state->movement->id_to_index[id] = -1;
        }
    }

    {
        if (state->entitiesReg->comp[id] & COMP_LIFETIME) {
            int idx = state->lifetime->id_to_index[id];
            int last = --state->lifetime->count;

            if (idx != last) {
                state->lifetime->ids[idx]   = state->lifetime->ids[last];
                state->lifetime->lifetime[idx]   = state->lifetime->lifetime[last];

                state->lifetime->id_to_index[state->lifetime->ids[idx]] = idx;
            }
            state->lifetime->id_to_index[id] = -1;
            printf("Lifetime count: %i\n", state->lifetime->count);
        }
    }

    {
        if (state->entitiesReg->comp[id] & COMP_RENDER) {
            int idx = state->render->id_to_index[id];
            int last = --state->render->count;

            if (idx != last) {
                state->render->ids[idx]   = state->render->ids[last];
                state->render->verts[idx]   = state->render->verts[last];
                state->render->vertCount[idx]   = state->render->vertCount[last];

                state->render->id_to_index[state->render->ids[idx]] = idx;
            }
            state->render->id_to_index[id] = -1;
            printf("Render count: %i\n", state->render->count);
        }
    }

    {
        if (state->entitiesReg->comp[id] & COMP_COLLISION) {
            int idx = state->collision->id_to_index[id];
            int last = --state->collision->count;

            if (idx != last) {
                state->collision->ids[idx]   = state->collision->ids[last];
                state->collision->size[idx]   = state->collision->size[last];

                state->collision->id_to_index[state->collision->ids[idx]] = idx;
            }
            state->collision->id_to_index[id] = -1;
            printf("Collision count: %i\n", state->collision->count);
        }
    }

    {
        if (state->entitiesReg->comp[id] & COMP_FLOATABLE) {
            int idx = state->floatable->id_to_index[id];
            int last = --state->floatable->count;

            if (idx != last) {
                state->floatable->ids[idx]   = state->floatable->ids[last];

                state->floatable->id_to_index[state->floatable->ids[idx]] = idx;
            }
            state->floatable->id_to_index[id] = -1;
            printf("Floatable count: %i\n", state->floatable->count);
        }
    }
}

static void CleanupDeadEntities(GameState *state) {
    for(int i = 0; i < state->entitiesReg->deleteCount; i++) {
        EntityID entity = state->entitiesReg->toDelete[i];
        printf("Entity to delete from systems: %i\n", entity);
        if(entity != -1) {
            RemoveEntityFromSystems(state, entity);
            state->entitiesReg->freeList[state->entitiesReg->freeCount++] = entity;
            state->entitiesReg->toDelete[i] = -1;
            state->entitiesReg->comp[entity] = COMP_NONE;
        }
    }
    state->entitiesReg->deleteCount = 0;
}
