#ifndef WORLD_QUERY_H
#define WORLD_QUERY_H

#include <engine/world/entity/component.h>
#include <engine/memory/list.h>
#include <inttypes.h>

#define TERM_MAX 10

// TODO: if component 0 is undefined, we can use tricks to get around
// storing sizes.
typedef struct Query {
  ComponentType with[TERM_MAX];
  uint32_t with_size;

  ComponentType without[TERM_MAX];
  uint32_t without_size;
} Query;

typedef struct QueryResult {
  List entities;
} QueryResult;

QueryResult Query_Execute(
    struct World* world,
    Query query);

#endif
