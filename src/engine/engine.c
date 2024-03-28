#include <engine/engine.h>
#include <engine_export.h>

#include <common/log.h>
#include <common/macros.h>
#include <common/status.h>
#include <common/v2.h>
#include <engine/render/render.h>
#include <engine/render/gizmos.h>
#include <engine/input/input.h>
#include <engine/render/asset_server.h>
#include <engine/sound/sound.h>
#include <engine/memory/memory.h>
#include <engine/world/world.h>
#include <platform/platform_runner.h>

static void InitializeGameState(GameState* game_state,
                                struct PlatformAPI* platform_api,
                                Memory* memory) {
  game_state->platform_api = platform_api;

  Arena asset_server_arena;
  Memory_InitializePermArena(&asset_server_arena, memory, MEGABYTES(64));
  Asset_ServerInitialize(&game_state->asset_server,
                         game_state->platform_api,
                         asset_server_arena);

  Asset_Handle handle;
  Status stat = Asset_ServerLoadAsset(&game_state->asset_server,
                                      AT_SHADER,
                                      "Z:\\shellhack\\test.txt",
                                      &handle);

  Status test = Asset_ServerLoadAsset(&game_state->asset_server,
                                      AT_SHADER,
                                      "Z:\\shellhack\\test.txt",
                                      &handle);

  Arena world_arena;
  Memory_InitializePermArena(&world_arena,memory, MEGABYTES(64));
  World_Initialize(&game_state->world,
                   world_arena);

  /*
  const float vp_w = (float) pixel_buffer->width;
  const float vp_h = (float) pixel_buffer->height;
  const V2 vp_c = (V2){ vp_w / 2.0f, vp_h / 2.0f };
  game_state->camera.viewport = Shape_RectCenterDims(
      vp_c, vp_w, vp_h);
  */

  game_state->is_initialized = true;
}

__declspec(dllexport)
Status Engine_Update(struct PlatformAPI* platform_api,
                     float dt_s,
                     Memory* memory,
                     Input* input,
                     SoundSampleBuffer* sound_buffer) {
  GameState* game_state = (GameState*) memory->permanent_storage;
  if (!game_state->is_initialized) {
    ASSERT(sizeof(GameState) <= memory->permanent_storage_capacity);

    // reserve first block of perm storage for game state.
    Arena ignored;
    ASSERT_OK(Memory_InitializePermArena(&ignored, memory, sizeof(GameState)));

    InitializeGameState(game_state, platform_api, memory);
  }

  Resources resources;
  resources.dt_s = dt_s;
  resources.input = input;
  World_Update(&game_state->world, &resources, memory);



  /*
  Render_FlushScreen(pixel_buffer, (Color){0.0f, 0.0f, 0.0f});
  Render_DrawEntitiesAroundCamera(
      pixel_buffer,
      &game_state->camera,
      &game_state->world);
  */

  return OK;
}
