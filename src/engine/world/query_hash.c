#include <engine/world/query_hash.h>

#include <common/bool.h>
#include <common/macros.h>
#include <engine/world/query.h>

// TODO: a better hash definitely exists.
static inline int32_t QueryHash_Hash(Query* key) {
  return key->with_size * key->without_size;
}

static bool Query_Equals(
    Query* a,
    Query* b) {
  if (a->with_size != b->with_size ||
      a->without_size != b->without_size) {
    return false;
  }
  for (uint32_t i = 0; i < a->with_size; i++) {
    if (a->with[i] != b->with[i]) { return false; }
  }
  for (uint32_t i = 0; i < a->without_size; i++) {
    if (a->without[i] != b->without[i]) { return false; }
  }
  return true;
}

void QueryHash_Insert(
    QueryResultHashMap* hash_map,
    Query key,
    QueryResult value) {
  int32_t index = QueryHash_Hash(&key);

  for (QueryResultHash* ptr = hash_map->table[index]; ptr != NULL; ptr = ptr->next) {
    ASSERT(!Query_Equals(&key, &ptr->key));
  }

  QueryResultHash* ptr = (QueryResultHash*) Allocator_AllocateBlock(
      hash_map->allocator, sizeof(QueryResultHash));
  ASSERT(ptr != NULL);
  ptr->key = key;
  ptr->value = value;

  ptr->next = hash_map->table[index];
  hash_map->table[index] = ptr;
}

QueryResult* QueryHash_Get(
    QueryResultHashMap* hash_map,
    Query key) {
  int32_t index = QueryHash_Hash(&key);
  for (QueryResultHash* ptr = hash_map->table[index]; ptr != NULL; ptr = ptr->next) {
    if (Query_Equals(&key, &ptr->key)) {
      return &ptr->value;
    }
  }
  return NULL;
}
