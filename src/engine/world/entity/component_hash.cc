#include <engine/world/entity/component_hash.h>

#include <common/status.h>
#include <common/macros.h>
#include <engine/memory/memory.h>
#include <engine/world/entity/entity.h>
#include <engine/world/entity/component.h>

// TODO: there might be a better scheme here, not sure.
static inline int32_t ComponentHash_Hash(
    ComponentHashMap* hash_map,
    EntityId entity_id) {
  return entity_id % ARRAY_SIZE(hash_map->table);
}

void ComponentHash_Insert(
    ComponentHashMap* hash_map,
    EntityId entity_id,
    Component component) {
  ASSERT(component.type == hash_map->type);

  ComponentHash to_store = {};
  to_store.entity_id = entity_id;
  to_store.component = component;

  int32_t index = ComponentHash_Hash(hash_map, entity_id);

  // validate only one component allowed per entity.
  for (ComponentHash* ptr = hash_map->table[index]; ptr != NULL; ptr = ptr->next) {
    ASSERT(ptr->entity_id != entity_id);
  }

  // find space to use.
  ComponentHash* ptr = NULL;
  if (hash_map->free_list != NULL) {
    ptr = hash_map->free_list;
    hash_map->free_list = hash_map->free_list->next;
  } else {
    ptr = (ComponentHash*)Arena_PushSize(hash_map->arena, sizeof(ComponentHash));
  }
  ASSERT(ptr != NULL);

  // insert.
  ptr->next = hash_map->table[index];
  hash_map->table[index] = ptr;
}

void ComponentHash_Remove(
    ComponentHashMap* hash_map,
    EntityId entity_id) {
  int32_t index = ComponentHash_Hash(hash_map, entity_id);
  for (ComponentHash* ptr = hash_map->table[index]; ptr != NULL; ptr = ptr->next) {
    if (ptr->next != NULL && ptr->next->entity_id == entity_id) {
      ComponentHash* to_free = ptr->next;
      ptr->next = to_free->next;
      to_free->next = hash_map->free_list;
      hash_map->free_list = to_free;
      break;
    }
  }
}

Component* ComponentHash_Get(
    ComponentHashMap* hash_map,
    EntityId entity_id) {
  int32_t index = ComponentHash_Hash(hash_map, entity_id);
  for (ComponentHash* ptr = hash_map->table[index]; ptr != NULL; ptr = ptr->next) {
    if (ptr->entity_id == entity_id) {
      return &ptr->component;
    }
  }
  return NULL;
}

ComponentHash* ComponentHashIter_First(
    ComponentHashMapIter* iter,
    ComponentHashMap* hash_map) {
  *iter = {};
  iter->hash_map = hash_map;
  return ComponentHashIter_Next(iter);
}

ComponentHash* ComponentHashIter_Next(
    ComponentHashMapIter* iter) {
  if (iter->ptr != NULL) {
    iter->ptr = iter->ptr->next;
    if (iter->ptr == NULL) {
      return ComponentHashIter_Next(iter);
    }
  } else {
    for (int32_t i = iter->index; i < ARRAY_SIZE(iter->hash_map->table); i++) {
      if (iter->hash_map->table[i] != NULL) {
        // NOTE: next time to loop should be after all elems are exhausted.
        iter->index = i + 1;
        iter->ptr = iter->hash_map->table[i];
        return iter->ptr;
      }
    }
  }
  return NULL;
}

bool ComponentHashIter_Done(
    ComponentHashMapIter* iter) {
  return iter->ptr == NULL;
}
