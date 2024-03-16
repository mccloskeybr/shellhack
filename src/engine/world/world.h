#ifndef ENGINE_WORLD_WORLD_H
#define ENGINE_WORLD_WORLD_H

#include <inttypes.h>
#include <engine/memory/memory.h>
#include <engine/input/input.h>
#include <engine/world/entity/component.h>
#include <engine/world/entity/component_hash.h>

struct World {
  ComponentHashMap component_maps[NUM_COMPONENT_TYPES];
  EntityId entity_count;

  Arena arena;
};

struct Resources {
  float dt_s;
  Input* input;
};

void World_Initialize(
    World* world,
    Memory* memory);

void World_Update(
    World* world,
    const Resources* const resources,
    Memory* memory);

#endif
