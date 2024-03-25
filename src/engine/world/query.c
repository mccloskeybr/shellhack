#include <engine/world/query.h>

#include <common/macros.h>
#include <engine/world/entity/component.h>
#include <engine/world/entity/component_hash.h>
#include <engine/world/world.h>
#include <inttypes.h>

static void Query_ExecuteWithFilter(
    List* entities,
    struct World* world,
    Query* query) {
  for (uint32_t i = 0; i < entities->size; i++) {
    EntityId test_eid = LIST_GET(entities, i, EntityId);
    for (uint32_t j = 0; j < query->with_size; j++) {
      Component* component = ComponentHash_Get(
          &world->component_maps[query->with[j]],
          test_eid);
      if (component == NULL) {
        LIST_REMOVE(entities, i);
        i--;
        break;
      }
    }
  }
}

static void Query_ExecuteWithoutFilter(
    List* entities,
    World* world,
    Query* query) {
  for (uint32_t i = 0; i < entities->size; i++) {
    EntityId test_eid = LIST_GET(entities, i, EntityId);
    for (uint32_t j = 0; j < query->without_size; j++) {
      Component* component = ComponentHash_Get(
          &world->component_maps[query->without[j]],
          test_eid);
      if (component != NULL) {
        LIST_REMOVE(entities, i);
        i--;
        break;
      }
    }
  }
}

QueryResult Query_Execute(
    struct World* world,
    Query query) {
  List entities = LIST_COPY(
      &world->allocator,
      &world->entities);
  Query_ExecuteWithFilter(&entities, world, &query);
  Query_ExecuteWithoutFilter(&entities, world, &query);

  QueryResult result;
  result.entities = entities;
  return result;
}
