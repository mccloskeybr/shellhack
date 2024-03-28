add_library(win32_graphics win32_graphics.c ../platform.h)
target_link_libraries(win32_graphics PRIVATE
  opengl32
	log
)

add_library(win32_sound win32_sound.c ../platform.h)
target_link_libraries(win32_sound PRIVATE
	log
)

add_library(win32_input win32_input.c ../platform.h)
target_link_libraries(win32_input PRIVATE
	log
)

add_library(win32_os win32_os.c ../platform.h)
target_link_libraries(win32_os PRIVATE
  platform_runner
	log
)

add_library(win32_hotload win32_hotload.c ../platform.h)
target_link_libraries(win32_os PRIVATE
	log
)

add_library(win32_stopwatch win32_stopwatch.c ../platform.h)

add_library(win32_io win32_io.c ../platform.h)

add_executable(win32_main WIN32 win32_main.c)
target_link_libraries(win32_main PRIVATE
	platform_runner
)
set_property(TARGET win32_main PROPERTY C_STANDARD 99)
