add_library(render render.h render.cc)
target_link_libraries(render PRIVATE
  world
	component_hash
)

add_library(gizmos gizmos.h gizmos.cc)
