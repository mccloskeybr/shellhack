#ifndef WORLD_QUERY_H
#define WORLD_QUERY_H

#include <engine/world/entity/component.h>
#include <engine/memory/list.h>
#include <inttypes.h>

#define TERM_MAX 10

typedef struct World World;

struct Query {
  ComponentType with[TERM_MAX];
  uint32_t with_size;

  ComponentType without[TERM_MAX];
  uint32_t without_size;
};

struct QueryResult {
  List entities;
};

QueryResult Query_Execute(
    World* world,
    Query query);

#endif
