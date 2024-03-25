include_directories(.)
add_subdirectory(win32)

add_library(platform_runner platform_runner.h platform_runner.c)
target_link_libraries(platform_runner PRIVATE
	log
	opengl32
	mem_util
	win32_graphics
	win32_sound
	win32_input
	win32_hotload
	win32_os
	win32_stopwatch
)
