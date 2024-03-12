include_directories(.)
add_subdirectory(win32)

add_library(platform_runner platform_runner.h platform_runner.cpp)
target_link_libraries(platform_runner PRIVATE
	win32_graphics
	win32_sound
	win32_input
	win32_memory
	win32_hotload
	win32_os
	win32_stopwatch
)
