#ifndef PLATFORM_H
#define PLATFORM_H

#include <common/status.h>
#include <engine/render/render.h>
#include <engine/input/input.h>
#include <engine/sound/sound.h>
#include <engine/memory/memory.h>
#include <inttypes.h>

// Input
struct Platform_InputDevice;
Status Platform_CreateInputDevice(struct Platform_InputDevice** device);
void Platform_UpdateControllerStates(
    const struct Platform_InputDevice* const device,
    Input* input);

// Graphics
struct Platform_Graphics;
Status Platform_InitializeGraphics(struct Platform_Graphics** window);
Status Platform_SwapBuffer(const struct Platform_Graphics* window);

// Audio
struct Platform_SoundDevice;
Status Platform_CreateSoundDevice(struct Platform_SoundDevice** device,
                                  int32_t samples_per_second,
                                  int32_t bytes_per_sample,
                                  int32_t sound_buffer_sample_size);
Status Platform_TryFillSoundBuffer(const SoundSampleBuffer* const samples,
                                   struct Platform_SoundDevice* device);

// File IO
struct Platform_FileHandle;
#define PLATFORM_FILE_OPEN(name) Status name( \
  struct Platform_FileHandle** file_handle, char* path)
#define PLATFORM_FILE_GET_SIZE(name) int32_t name( \
  struct Platform_FileHandle* file_handle)
#define PLATFORM_FILE_READ(name) Status name( \
  struct Platform_FileHandle* file_handle, void* dest, int32_t size)
#define PLATFORM_FILE_CLOSE(name) void name( \
  struct Platform_FileHandle* file_handle)
PLATFORM_FILE_OPEN(Platform_FileOpen);
PLATFORM_FILE_GET_SIZE(Platform_FileGetSize);
PLATFORM_FILE_READ(Platform_FileRead);
PLATFORM_FILE_CLOSE(Platform_FileClose);

// Engine hotload
struct Platform_EngineLib;
#define ENGINE_UPDATE(name) Status name(  \
    struct PlatformAPI* platform_api,     \
    float dt_s,			                      \
    Memory* memory,   	                  \
    Input* input,			                    \
    SoundSampleBuffer* sound_buffer)
typedef ENGINE_UPDATE(EngineUpdateFn);
Status Platform_CreateEngineLib(char* lib_file_path,
                                struct Platform_EngineLib** engine_lib);
Status Platform_MaybeReloadEngineLib(struct Platform_EngineLib* engine_lib);
EngineUpdateFn* Platform_GetEngineUpdateFn(struct Platform_EngineLib* engine_lib);

// Timing
struct Platform_Stopwatch;
Status Platform_CreateStopwatch(struct Platform_Stopwatch** stopwatch);
void Platform_CloseStopwatch(struct Platform_Stopwatch* stopwatch);
void Platform_ResetStopwatch(struct Platform_Stopwatch** stopwatch);
void Platform_UpdateStopwatch(struct Platform_Stopwatch** stopwatch);
float Platform_SecondsElapsed(
    const struct Platform_Stopwatch* const stopwatch);

// Misc. OS
Status Platform_InitializeMemory(void** block, uint64_t size);
void Platform_HandleEvents();
float Platform_GetMonitorRefreshRate();
void Platform_SleepMs(int32_t duration_s);

// Vtable
typedef PLATFORM_FILE_OPEN(Platform_FileOpen_Fn);
typedef PLATFORM_FILE_GET_SIZE(Platform_FileGetSize_Fn);
typedef PLATFORM_FILE_READ(Platform_FileRead_Fn);
typedef PLATFORM_FILE_CLOSE(Platform_FileClose_Fn);
typedef struct PlatformAPI {
  Platform_FileOpen_Fn* file_open_fn;
  Platform_FileGetSize_Fn* file_get_size_fn;
  Platform_FileRead_Fn* file_read_fn;
  Platform_FileClose_Fn* file_close_fn;
} PlatformAPI;

#endif
