include_directories(.)
add_subdirectory(entity)

add_library(world world.h world.cc)
target_link_libraries(world PRIVATE
  allocator
  component_hash
  list
  log
  memory
)
