#ifndef WORLD_ENTITY_COMPONENT_H
#define WORLD_ENTITY_COMPONENT_H

#include <common/v2.h>
#include <engine/world/entity/entity.h>

enum ComponentType {
  COMPONENT_TYPE_SPATIAL,
  COMPONENT_TYPE_COLLISION,
  COMPONENT_TYPE_PLAYER_CONTROLLER,

  COMPONENT_TYPE_END_MARKER
};
#define NUM_COMPONENT_TYPES COMPONENT_TYPE_END_MARKER

struct SpatialComponent {
  V2 world_pos;
};

struct CollisionComponent {
  bool collides;
  float radius;
};

struct PlayerControllerComponent {
  EntityId player_id;
};

struct Component {
  ComponentType type;
  union {
    SpatialComponent spatial;
    CollisionComponent collision;
    PlayerControllerComponent player_controller;
  };
};

#endif
