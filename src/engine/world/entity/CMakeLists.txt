add_library(component_hash component_hash.h component_hash.cc)
target_link_libraries(component_hash PRIVATE
	allocator
)
