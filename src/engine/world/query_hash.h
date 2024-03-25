#ifndef WORLD_QUERY_HASH_H
#define WORLD_QUERY_HASH_H

#include <engine/world/query.h>
#include <engine/memory/allocator.h>

typedef struct QueryResultHash {
  Query key;
  QueryResult value;
  struct QueryResultHash* next;
} QueryResultHash;

typedef struct QueryResultHashMap {
  QueryResultHash* table[258];
  Allocator* allocator;
} QueryResultHashMap;

void QueryHash_Insert(
    QueryResultHashMap* hash_map,
    Query key,
    QueryResult value);

QueryResult* QueryHash_Get(
    QueryResultHashMap* hash_map,
    Query key);

#endif
