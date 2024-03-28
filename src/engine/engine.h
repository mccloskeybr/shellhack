#ifndef ENGINE
#define ENGINE

#include <engine_export.h>

#include <common/bool.h>
#include <common/v2.h>
#include <common/status.h>
#include <engine/render/render.h>
#include <engine/render/camera.h>
#include <engine/input/input.h>
#include <engine/render/asset_server.h>
#include <engine/sound/sound.h>
#include <engine/memory/memory.h>
#include <engine/world/world.h>

typedef struct GameState {
  bool is_initialized;

  struct PlatformAPI* platform_api;

  Camera camera;
  World world;
  Asset_Server asset_server;

} GameState;

__declspec(dllexport)
Status Engine_Update(struct PlatformAPI* platform_api,
                     float dt_s,
                     Memory* memory,
                     Input* input,
                     SoundSampleBuffer* sound_buffer);

#endif
