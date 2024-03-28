add_library(render render.h render.c)
target_link_libraries(render PRIVATE
	component_hash
  world
)

add_library(asset_server asset_server.h asset_server.c)
target_link_libraries(asset_server PRIVATE
  allocator
)

add_library(gizmos gizmos.h gizmos.c)
