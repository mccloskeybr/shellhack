#include <engine/world/entity/component_hash.h>

#include <common/status.h>
#include <common/macros.h>
#include <engine/memory/allocator.h>
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
  to_store.ec_pair.entity_id = entity_id;
  to_store.ec_pair.component = component;

  int32_t index = ComponentHash_Hash(hash_map, entity_id);

  // validate only one component allowed per entity.
  for (ComponentHash* ptr = hash_map->table[index]; ptr != NULL; ptr = ptr->next) {
    ASSERT(ptr->ec_pair.entity_id != entity_id);
  }

  // find space to use.
  ComponentHash* ptr = (ComponentHash*) Allocator_AllocateBlock(
      hash_map->allocator, sizeof(ComponentHash));
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
    if (ptr->next != NULL && ptr->next->ec_pair.entity_id == entity_id) {
      ComponentHash* to_free = ptr->next;
      ptr->next = ptr->next->next;
      Allocator_FreeBlock(hash_map->allocator, to_free);
      break;
    }
  }
}

EntityIdComponentPair* ComponentHash_Get(
    ComponentHashMap* hash_map,
    EntityId entity_id) {
  int32_t index = ComponentHash_Hash(hash_map, entity_id);
  for (ComponentHash* ptr = hash_map->table[index]; ptr != NULL; ptr = ptr->next) {
    if (ptr->ec_pair.entity_id == entity_id) {
      return &ptr->ec_pair;
    }
  }
  return NULL;
}

EntityIdComponentCollection ComponentHash_Collect(
    ComponentHashMap* hash_map) {
  EntityIdComponentCollection collection = {};
  for (int32_t i = 0; i < ARRAY_SIZE(hash_map->table); i++) {
    if (hash_map->table[i] == NULL) {
      continue;
    }
    for (ComponentHash* ptr = hash_map->table[i]; ptr != NULL; ptr = ptr->next) {
      collection.ec_pairs[collection.size++] = &ptr->ec_pair;
    }
  }
  return collection;
}

EntityIdComponentPair* CHM_IterFirst(
    CHM_Iter* iter,
    ComponentHashMap* hash_map) {
  *iter = {};
  iter->hash_map = hash_map;
  return CHM_IterNext(iter);
}

EntityIdComponentPair* CHM_IterNext(
    CHM_Iter* iter) {
  if (iter->ptr != NULL) {
    iter->ptr = iter->ptr->next;
    if (iter->ptr != NULL) {
      return &iter->ptr->ec_pair;
    } else {
      return CHM_IterNext(iter);
    }
  } else {
    for (int32_t i = iter->index; i < ARRAY_SIZE(iter->hash_map->table); i++) {
      if (iter->hash_map->table[i] != NULL) {
        // NOTE: next time to loop should be after all elems are exhausted.
        iter->index = i + 1;
        iter->ptr = iter->hash_map->table[i];
        return &iter->ptr->ec_pair;
      }
    }
  }
  return NULL;
}

bool CHM_IterDone(
    CHM_Iter* iter) {
  return iter->ptr == NULL;
}
