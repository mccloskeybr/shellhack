add_library(render render.h render.c)
target_link_libraries(render PRIVATE
	component_hash
  mem_util
  world
)

add_library(gizmos gizmos.h gizmos.c)
