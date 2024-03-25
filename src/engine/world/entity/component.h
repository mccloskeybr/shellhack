#ifndef WORLD_ENTITY_COMPONENT_H
#define WORLD_ENTITY_COMPONENT_H

#include <common/bool.h>
#include <common/v2.h>
#include <engine/world/entity/entity.h>

typedef enum ComponentType {
  COMPONENT_TYPE_SPATIAL,
  COMPONENT_TYPE_COLLISION,
  COMPONENT_TYPE_PLAYER_CONTROLLER,

  COMPONENT_TYPE_END_MARKER
} ComponentType;
#define NUM_COMPONENT_TYPES COMPONENT_TYPE_END_MARKER

typedef struct SpatialComponent {
  V2 world_pos;
} SpatialComponent;

typedef struct CollisionComponent {
  bool collides;
  float radius;
} CollisionComponent;

typedef struct PlayerControllerComponent {
  EntityId player_id;
} PlayerControllerComponent;

typedef struct Component {
  ComponentType type;
  union {
    SpatialComponent spatial;
    CollisionComponent collision;
    PlayerControllerComponent player_controller;
  };
} Component;

#endif
