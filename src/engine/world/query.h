#ifndef WORLD_QUERY_H
#define WORLD_QUERY_H

#include <engine/world/entity/component.h>
#include <engine/world/world.h>

#define TERM_MAX 10

struct Query {
  ComponentType with[TERM_MAX];
  ComponentType without[TERM_MAX];
};

struct QueryResults {
  // think of a better way to store this.
  // could just throw it in the arena? resize memes?
  EntityId[MAX_ENTITIES] entities;
};

struct QueryResultsHash {
  Query query;
  QueryResults results;
  QueryResultsHash* next;
};

struct QueryResultsHashMap {
  QueryResultsHash* table[258];
  QueryResultsHash* free_list;
  Arena* arena;
};

// this gets stored in the world.
struct QueryCache {
  // think of a better way to store this.
  // could just throw it in the arena? resize memes?
  QueryResultsHashMap* query_results_map[10];
  Arena* arena;
};

QueryResults Query_Execute(
    World* world,
    Query query);

#endif
