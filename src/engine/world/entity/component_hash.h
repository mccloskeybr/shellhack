#ifndef WORLD_ENTITY_COMPONENT_HASH_H
#define WORLD_ENTITY_COMPONENT_HASH_H

#include <engine/memory/memory.h>
#include <engine/world/entity/entity.h>
#include <engine/world/entity/component.h>

typedef struct _ComponentHash {
  EntityId entity_id;
  Component component;
  _ComponentHash* next;
} ComponentHash;

typedef struct _ComponentHashMap {
  ComponentType type;
  ComponentHash* table[256];
  ComponentHash* free_list;
  Arena* arena;
} ComponentHashMap;

typedef struct _ComponentHashMapIter {
  int32_t index;
  ComponentHash* ptr;
  ComponentHashMap* hash_map;
} ComponentHashMapIter;

void ComponentHash_Insert(
    ComponentHashMap* hash_map,
    EntityId entity_id,
    Component component);

void ComponentHash_Remove(
    ComponentHashMap* hash_map,
    EntityId entity_id);

Component* ComponentHash_Get(
    ComponentHashMap* hash_map,
    EntityId entity_id);

ComponentHash* ComponentHashIter_First(
    ComponentHashMapIter* iter,
    ComponentHashMap* hash_map);

ComponentHash* ComponentHashIter_Next(
    ComponentHashMapIter* iter);

bool ComponentHashIter_Done(
    ComponentHashMapIter* iter);

#endif

