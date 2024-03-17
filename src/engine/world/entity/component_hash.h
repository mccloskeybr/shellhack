#ifndef WORLD_ENTITY_COMPONENT_HASH_H
#define WORLD_ENTITY_COMPONENT_HASH_H

#include <engine/memory/allocator.h>
#include <engine/world/entity/entity.h>
#include <engine/world/entity/component.h>

struct ComponentHash {
  EntityIdComponentPair ec_pair;
  ComponentHash* next;
};

struct ComponentHashMap {
  ComponentType type;
  ComponentHash* table[256];
  Allocator* allocator;
};

struct CHM_Iter {
  int32_t index;
  ComponentHash* ptr;
  ComponentHashMap* hash_map;
};

void ComponentHash_Insert(
    ComponentHashMap* hash_map,
    EntityId entity_id,
    Component component);

EntityIdComponentPair* ComponentHash_Get(
    ComponentHashMap* hash_map,
    EntityId entity_id);

void ComponentHash_Remove(
    ComponentHashMap* hash_map,
    EntityId entity_id);

// NOTE: collection is spatially inefficient compared to the iterator.
// e.g. when collecting multiple components simultaneously.
EntityIdComponentCollection ComponentHash_Collect(
    ComponentHashMap* hash_map);

EntityIdComponentPair* CHM_IterFirst(
    CHM_Iter* iter,
    ComponentHashMap* hash_map);

EntityIdComponentPair* CHM_IterNext(
    CHM_Iter* iter);

bool CHM_IterDone(
    CHM_Iter* iter);

#endif
