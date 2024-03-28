#include <engine/world/entity/component_hash.h>

#include <common/status.h>
#include <common/macros.h>
#include <engine/memory/allocator.h>
#include <engine/world/entity/entity.h>
#include <engine/world/entity/component.h>

// TODO: there might be a better scheme here, not sure.
static inline int32_t ComponentHash_Hash(
    ComponentHashMap* hash_map,
    EntityId key) {
  return key % ARRAY_SIZE(hash_map->table);
}

void ComponentHash_Insert(
    ComponentHashMap* hash_map,
    EntityId key,
    Component value) {
  ASSERT(value.type == hash_map->type);
  int32_t index = ComponentHash_Hash(hash_map, key);

  // validate only one component allowed per entity.
  for (ComponentHash* ptr = hash_map->table[index]; ptr != NULL; ptr = ptr->next) {
    ASSERT(ptr->key != key);
  }

  ComponentHash* ptr = (ComponentHash*) Allocator_AllocateBlock(
      hash_map->allocator, sizeof(ComponentHash));
  ASSERT(ptr != NULL);
  ptr->key = key;
  ptr->value = value;
  ptr->next = hash_map->table[index];
  hash_map->table[index] = ptr;
}

void ComponentHash_Remove(ComponentHashMap* hash_map, EntityId key) {
  int32_t index = ComponentHash_Hash(hash_map, key);
  for (ComponentHash* ptr = hash_map->table[index]; ptr != NULL; ptr = ptr->next) {
    if (ptr->next != NULL && ptr->next->key == key) {
      ComponentHash* to_free = ptr->next;
      ptr->next = ptr->next->next;
      Allocator_FreeBlock(hash_map->allocator, to_free);
      break;
    }
  }
}

Component* ComponentHash_Get(ComponentHashMap* hash_map, EntityId key) {
  int32_t index = ComponentHash_Hash(hash_map, key);
  for (ComponentHash* ptr = hash_map->table[index]; ptr != NULL; ptr = ptr->next) {
    if (ptr->key == key) {
      return &ptr->value;
    }
  }
  return NULL;
}

ComponentHash* CHM_IterFirst(CHM_Iter* iter, ComponentHashMap* hash_map) {
  *iter = (CHM_Iter){};
  iter->hash_map = hash_map;
  return CHM_IterNext(iter);
}

ComponentHash* CHM_IterNext(CHM_Iter* iter) {
  if (iter->ptr != NULL) {
    iter->ptr = iter->ptr->next;
    if (iter->ptr != NULL) {
      return iter->ptr;
    } else {
      return CHM_IterNext(iter);
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

bool CHM_IterDone(CHM_Iter* iter) {
  return iter->ptr == NULL;
}
