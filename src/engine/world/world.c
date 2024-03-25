#include <engine/world/world.h>

#include <common/log.h>
#include <common/macros.h>
#include <common/mem_util.h>
#include <engine/input/input.h>
#include <engine/memory/memory.h>
#include <engine/memory/allocator.h>
#include <engine/memory/list.h>
#include <engine/world/entity/entity.h>
#include <engine/world/entity/component.h>
#include <engine/world/entity/component_hash.h>
#include <engine/world/query.h>

static EntityId
World_AddEntity(World* world) {
  EntityId entity_id = world->next_entity_id++;
  LIST_ADD(&world->entities, entity_id, EntityId);
  return entity_id;
}

void World_InsertComponent(
    World* world,
    EntityId entity_id,
    Component component) {
  ComponentHash_Insert(
      &world->component_maps[component.type],
      entity_id,
      component);
}

static void InitializePlayer(World* world) {
  EntityId player_id = World_AddEntity(world);

  Component spatial_component;
  spatial_component.type = COMPONENT_TYPE_SPATIAL;
  spatial_component.spatial.world_pos = (V2){100.0f, 100.0f};
  World_InsertComponent(world, player_id, spatial_component);

  Component collision_component;
  collision_component.type = COMPONENT_TYPE_COLLISION;
  collision_component.collision.collides = true;
  collision_component.collision.radius = 50.0f;
  World_InsertComponent(world, player_id, collision_component);

  EntityId player_controller_id = World_AddEntity(world);
  Component player_controller_component;
  player_controller_component.type = COMPONENT_TYPE_PLAYER_CONTROLLER;
  player_controller_component.player_controller.player_id = player_id;
  World_InsertComponent(world, player_controller_id, player_controller_component);

}

static void UpdatePlayerController(
    World* world,
    const Resources* resources) {
  const Controller* const controller = &resources->input->controllers[1];
  if (!controller->is_connected) {
    return;
  }

  Query pc_query = {};
  pc_query.with[TERM_MAX] = (ComponentType){COMPONENT_TYPE_PLAYER_CONTROLLER};
  pc_query.with_size = 1;
  QueryResult* pc_query_results = World_Query(world, pc_query);

  Query spatial_query = {};
  spatial_query.with[TERM_MAX] = (ComponentType){COMPONENT_TYPE_SPATIAL};
  spatial_query.with_size = 1;
  QueryResult* spatial_query_results = World_Query(world, spatial_query);

  for (uint32_t i = 0; i < pc_query_results->entities.size; i++) {
    for (uint32_t j = 0; j < spatial_query_results->entities.size; j++) {
      EntityId eid = LIST_GET(&spatial_query_results->entities, j, EntityId);
      Component* spatial =
        ComponentHash_Get(&world->component_maps[COMPONENT_TYPE_SPATIAL], eid);

      V2 dir = {};
      if (controller->move_up.is_pressed) {
        dir = V2_Add(dir, (V2){0.0f, 1.0f});
      }
      if (controller->move_down.is_pressed) {
        dir = V2_Add(dir, (V2){0.0f, -1.0f});
      }
      if (controller->move_left.is_pressed) {
        dir = V2_Add(dir, (V2){-1.0f, 0.0f});
      }
      if (controller->move_right.is_pressed) {
        dir = V2_Add(dir, (V2){1.0f, 0.0f});
      }
      if (V2_Length(dir) > 0.0f) {
        dir = V2_Normalize(dir);
      }

      V2 vel = V2_ScalarMult(50.0f * resources->dt_s, dir);
      spatial->spatial.world_pos = V2_Add(spatial->spatial.world_pos, vel);

      LOG_INFO("player world pos <x: %0.1f, y: %.1f>",
          spatial->spatial.world_pos.x,
          spatial->spatial.world_pos.y);
    }
  }
}

void World_Initialize(
    World* world,
    Memory* memory) {
  Arena world_arena;
  ASSERT_OK(Memory_InitializePermArena(
        &world_arena, memory, MEGABYTES(64)));
  ASSERT_OK(Allocator_Init(
        &world->allocator, world_arena));

  world->entities = LIST_CREATE(&world->allocator, EntityId);

  for (int32_t i = 0; i < ARRAY_SIZE(world->component_maps); i++) {
    ComponentHashMap* map = &world->component_maps[i];
    Memory_ZeroRegion(map, sizeof(*map));
    map->type = (ComponentType)i;
    map->allocator = &world->allocator;
  }

  QueryResultHashMap* query_cache = &world->query_cache;
  Memory_ZeroRegion(query_cache, sizeof(*query_cache));
  query_cache->allocator = &world->allocator;

  InitializePlayer(world);
}

void World_Update(
    World* world,
    const Resources* const resources,
    Memory* memory) {
  UpdatePlayerController(world, resources);
}

QueryResult* World_Query(
    World* world,
    Query query) {
  QueryResult* cached_results = QueryHash_Get(&world->query_cache, query);
  if (cached_results != NULL) { return cached_results; }

  QueryResult results = Query_Execute(world, query);
  QueryHash_Insert(&world->query_cache, query, results);
  return QueryHash_Get(&world->query_cache, query);
}
