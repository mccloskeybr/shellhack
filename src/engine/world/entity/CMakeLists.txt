add_library(component_hash component_hash.h component_hash.c)
target_link_libraries(component_hash PRIVATE
	allocator
	mem_util
)
