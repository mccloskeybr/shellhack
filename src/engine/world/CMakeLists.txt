include_directories(.)
add_subdirectory(entity)

add_library(world world.h world.c)
target_link_libraries(world PRIVATE
  allocator
  component_hash
  list
  log
  memory
  mem_util
  query_hash
  query
)

add_library(query query.h query.c)
target_link_libraries(query PRIVATE
  component_hash
  list
  world
)

add_library(query_hash query_hash.h query_hash.c)
target_link_libraries(query_hash PRIVATE
  allocator
  query
)
