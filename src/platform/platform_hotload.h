#ifndef PLATFORM_HOTLOAD
#define PLATFORM_HOTLOAD

#include <common/status.h>
#include <engine/render/render.h>
#include <engine/input/input.h>
#include <engine/sound/sound.h>
#include <engine/memory/memory.h>

#define ENGINE_UPDATE(name) Status name(  \
    float dt_s,			                      \
    Memory* memory,   	                  \
    Input* input,			                    \
    PixelBuffer* pixel_buffer,            \
    SoundSampleBuffer* sound_buffer);
typedef ENGINE_UPDATE(EngineUpdateFn);

struct Platform_EngineLib;

Status
Platform_CreateEngineLib(
    char* lib_file_path,
    Platform_EngineLib** engine_lib);

Status
Platform_MaybeReloadEngineLib(
    Platform_EngineLib* engine_lib);

EngineUpdateFn*
GetEngineUpdateFn(
    Platform_EngineLib* engine_lib);

#endif
