add_library(win32_graphics win32_graphics.cpp ../platform_graphics.h)
target_link_libraries(win32_graphics PRIVATE
	platform_runner
)

add_library(win32_sound win32_sound.cpp ../platform_sound.h)
target_link_libraries(win32_sound PRIVATE
	platform_runner
)

add_library(win32_input win32_input.cpp ../platform_input.h)
target_link_libraries(win32_input PRIVATE
	platform_runner
)

add_library(win32_os win32_os.cpp ../platform_os.h)
target_link_libraries(win32_os PRIVATE
	platform_runner
)

add_library(win32_hotload win32_hotload.cpp ../platform_hotload.h)

add_library(win32_stopwatch win32_stopwatch.cpp ../platform_stopwatch.h)

add_executable(win32_main WIN32 win32_main.cpp)
target_link_libraries(win32_main PRIVATE
	platform_runner
)
