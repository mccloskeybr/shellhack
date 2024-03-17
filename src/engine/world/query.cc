#include <engine/world/query.h>

#include <engine/world/entity/component.h>
#include <engine/world/world.h>

EntityIdComponentCollection Query_Execute(
    World* world,
    Query query) {
  ASSERT(ARRAY_SIZE(query.with) > 0);

  // collect the initial round that we can hash against, since collecting is expensive.
  EntityIdComponentCollection results = ComponentHash_Collect(
      &world->component_maps[query.with[0]]);

  // loop over all of the requested inclusion components
  for (int32_t i = 1; i < ARRAY_SIZE(query.with); i++) {
    int32_t intersection_size = 0;
    EntityIdComponentCollection intersection = results;

    // loop over all of the entities we're currently keeping track of.
    for (int j = 0; j < results.size; j++) {

      // see if it also has the next requested component id
      EntityId t_eid = results.ec_pairs[j]->entity_id;
      EntityIdComponentPair t_pair =
        ComponentHash_Get(&world->component_maps[query.with[i]], t_eid;
      if (t_pair != NULL) {
        // intersection[intersection_size++] =
      }
    }
    results = intersection;
  }
}
