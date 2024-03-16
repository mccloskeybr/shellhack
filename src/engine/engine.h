#ifndef ENGINE
#define ENGINE

#include <engine_export.h>

#include <common/v2.h>
#include <common/status.h>
#include <engine/render/render.h>
#include <engine/render/camera.h>
#include <engine/input/input.h>
#include <engine/sound/sound.h>
#include <engine/memory/memory.h>
#include <engine/world/world.h>

struct GameState {
  bool is_initialized = false;

  Camera camera;
  World world;
};

extern "C" ENGINE_EXPORT Status
Engine_Update(
    float dt_s,
    Memory* memory,
    Input* input,
    PixelBuffer* pixel_buffer,
    SoundSampleBuffer* sound_buffer);

#endif
