#ifndef ENGINE_WORLD_WORLD_H
#define ENGINE_WORLD_WORLD_H

#include <inttypes.h>
#include <engine/memory/allocator.h>
#include <engine/memory/list.h>
#include <engine/input/input.h>
#include <engine/world/entity/component.h>
#include <engine/world/entity/component_hash.h>
#include <engine/world/query_hash.h>
#include <engine/world/query.h>

typedef struct World {
  List entities;
  EntityId next_entity_id;

  ComponentHashMap component_maps[NUM_COMPONENT_TYPES];
  QueryResultHashMap query_cache;

  Allocator allocator;
} World;

typedef struct Resources {
  float dt_s;
  Input* input;
} Resources;

void World_Initialize(World* world, Arena world_arena);
void World_Update(
    World* world, const Resources* const resources, Memory* memory);
QueryResult* World_Query(World* world, Query query);

#endif
