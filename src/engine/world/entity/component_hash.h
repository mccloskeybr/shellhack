#ifndef WORLD_ENTITY_COMPONENT_HASH_H
#define WORLD_ENTITY_COMPONENT_HASH_H

#include <engine/memory/allocator.h>
#include <engine/world/entity/entity.h>
#include <engine/world/entity/component.h>

typedef struct ComponentHash {
  EntityId key;
  Component value;
  struct ComponentHash* next;
} ComponentHash;

typedef struct ComponentHashMap {
  ComponentType type;
  ComponentHash* table[256];
  Allocator* allocator;
} ComponentHashMap;

typedef struct CHM_Iter {
  int32_t index;
  ComponentHash* ptr;
  ComponentHashMap* hash_map;
} CHM_Iter;

void ComponentHash_Insert(
    ComponentHashMap* hash_map,
    EntityId key,
    Component value);

Component* ComponentHash_Get(
    ComponentHashMap* hash_map,
    EntityId key);

void ComponentHash_Remove(
    ComponentHashMap* hash_map,
    EntityId key);

ComponentHash* CHM_IterFirst(
    CHM_Iter* iter,
    ComponentHashMap* hash_map);

ComponentHash* CHM_IterNext(
    CHM_Iter* iter);

bool CHM_IterDone(
    CHM_Iter* iter);

#endif
