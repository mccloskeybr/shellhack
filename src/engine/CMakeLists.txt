add_library(engine SHARED engine.h engine.cpp)
target_link_libraries(engine PRIVATE
	platform_runner
)

generate_export_header(engine
	EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/engine_export.h
	BASE_NAME engine
	EXPORT_MACRO_NAME ENGINE_EXPORT)
target_include_directories(engine PUBLIC ${CMAKE_BINARY_DIR}/exports)

string(RANDOM LENGTH 5 rand)
string(CONCAT pdb_name "engine_" ${rand})
set_target_properties(engine PROPERTIES
	PDB_NAME ${pdb_name})
