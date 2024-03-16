#include <engine/engine.h>
#include <engine_export.h>

#include <common/log.h>
#include <common/status.h>
#include <common/v2.h>
#include <engine/render/render.h>
#include <engine/render/gizmos.h>
#include <engine/input/input.h>
#include <engine/sound/sound.h>
#include <engine/memory/memory.h>
#include <engine/world/world.h>
#include <platform/platform_runner.h>

static void
InitializeGameState(
    GameState* game_state,
    Memory* memory,
    const PixelBuffer* const pixel_buffer) {
  // the arena implicitly starts at the beginning of perm storage.
  Arena ignored;
  ASSERT_OK(Memory_InitializePermArena(
        &ignored,
        memory,
        sizeof(GameState)));

  const float vp_w = (float) pixel_buffer->width;
  const float vp_h = (float) pixel_buffer->height;
  game_state->camera.viewport = RectangleCenterDims(
      { vp_w / 2.0f, vp_h / 2.0f }, vp_w, vp_h);

  World_Initialize(&game_state->world, memory);

  game_state->is_initialized = true;
}

extern "C" ENGINE_EXPORT Status
Engine_Update(
    float dt_s,
    Memory* memory,
    Input* input,
    PixelBuffer* pixel_buffer,
    SoundSampleBuffer* sound_buffer) {

  GameState* game_state = (GameState*) memory->permanent_storage;
  if (!game_state->is_initialized) {
    ASSERT(sizeof(GameState) <= memory->permanent_storage_capacity);
    InitializeGameState(game_state, memory, pixel_buffer);
  }

  Resources resources;
  resources.dt_s = dt_s;
  resources.input = input;
  World_Update(&game_state->world, &resources, memory);

  Render_FlushScreen(pixel_buffer, {0.0f, 0.0f, 0.0f});
  Render_DrawEntitiesAroundCamera(
      pixel_buffer,
      &game_state->camera,
      &game_state->world);

  return OK;
}
