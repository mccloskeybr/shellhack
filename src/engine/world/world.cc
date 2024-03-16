#include <engine/world/world.h>

#include <common/log.h>
#include <common/macros.h>
#include <engine/input/input.h>
#include <engine/memory/memory.h>
#include <engine/world/entity/entity.h>
#include <engine/world/entity/component.h>
#include <engine/world/entity/component_hash.h>

static EntityId
World_AddEntity(World* world) {
  return world->entity_count++;
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
  spatial_component.spatial.world_pos = {100.0f, 100.0f};
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

void World_Initialize(
    World* world,
    Memory* memory) {
  ASSERT_OK(Memory_InitializePermArena(
        &world->arena, memory, MEGABYTES(64)));
  for (int32_t i = 0; i < ARRAY_SIZE(world->component_maps); i++) {
    ComponentHashMap* map = &world->component_maps[i];
    *map = {};
    map->type = (ComponentType)i;
    map->arena = &world->arena;
  }

  InitializePlayer(world);
}

static void UpdatePlayerController(
    World* world,
    const Resources* resources) {
  const Controller* const controller = &resources->input->controllers[1];
  if (!controller->is_connected) {
    return;
  }

  ComponentHashMapIter iter_1;
  for (ComponentHashIter_First(
        &iter_1,
        &world->component_maps[COMPONENT_TYPE_PLAYER_CONTROLLER]);
      !ComponentHashIter_Done(&iter_1);
      ComponentHashIter_Next(&iter_1)) {

    ComponentHashMapIter iter_2;
    for (ComponentHashIter_First(
          &iter_2,
          &world->component_maps[COMPONENT_TYPE_SPATIAL]);
        !ComponentHashIter_Done(&iter_2);
        ComponentHashIter_Next(&iter_2)) {

      V2 dir = {};
      if (controller->move_up.is_pressed) {
        dir += {0.0f, 1.0f};
      }
      if (controller->move_down.is_pressed) {
        dir += {0.0f, -1.0f};
      }
      if (controller->move_left.is_pressed) {
        dir += {-1.0f, 0.0f};
      }
      if (controller->move_right.is_pressed) {
        dir += {1.0f, 0.0f};
      }
      if (Length(dir) > 0.0f) {
        dir = Normalize(dir);
      }

      V2 vel = dir * 50.0f * resources->dt_s;
      iter_2.ptr->component.spatial.world_pos += vel;

      LOG_INFO("player world pos <x: %0.1f, y: %.1f>",
          iter_2.ptr->component.spatial.world_pos.x,
          iter_2.ptr->component.spatial.world_pos.y);
    }
  }
}


void World_Update(
    World* world,
    const Resources* const resources,
    Memory* memory) {
  UpdatePlayerController(world, resources);
}
