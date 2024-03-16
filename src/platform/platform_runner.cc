#include <platform/platform_runner.h>

#include <common/log.h>
#include <common/status.h>
#include <common/macros.h>
#include <engine/render/render.h>
#include <engine/input/input.h>
#include <engine/sound/sound.h>
#include <engine/memory/memory.h>
#include <platform/platform_os.h>
#include <platform/platform_sound.h>
#include <platform/platform_input.h>
#include <platform/platform_graphics.h>
#include <platform/platform_stopwatch.h>
#include <platform/platform_hotload.h>
#include <math.h>

#define NULL 0

static bool global_running;

static Status
InitializeMemory(Memory* memory) {
  *memory = {};
  memory->permanent_storage_capacity = MEGABYTES(256);
  memory->transient_storage_capacity = GIGABYTES(1);

  void* block;
  RETURN_IF_ERROR(Platform_InitializeMemory(
        &block,
        memory->permanent_storage_capacity + memory->transient_storage_capacity));
  memory->permanent_storage = block;
  memory->transient_storage =
    (uint8_t*) memory->permanent_storage + memory->permanent_storage_capacity;

  return OK;
}

static Status
InitializeInput(
    Platform_InputDevice** input_device,
    Input* input) {

  RETURN_IF_ERROR(Platform_CreateInputDevice(input_device));
  return OK;
}

static Status
InitializeGraphics(
    Platform_Window** window,
    PixelBuffer* pixel_buffer) {

  RETURN_IF_ERROR(Platform_CreateWindow(window));

  pixel_buffer->width = 1280;
  pixel_buffer->height = 720;
  void* block;
  RETURN_IF_ERROR(Platform_InitializeMemory(
        &block,
        pixel_buffer->width * pixel_buffer->height * sizeof(Pixel)));
  pixel_buffer->buffer = (Pixel*)block;

  return OK;
}

static Status
InitializeAudio(
    Platform_SoundDevice** sound_device,
    SoundSampleBuffer* sound_sample_buffer) {

  RETURN_IF_ERROR(Platform_CreateSoundDevice(
        sound_device,
        48000,
        sizeof(int16_t) * 2,
        48000));
  ASSERT(sound_device != NULL);

  sound_sample_buffer->buffer_size = 48000 / 30;
  void* block;
  RETURN_IF_ERROR(Platform_InitializeMemory(
        &block,
        sound_sample_buffer->buffer_size * sizeof(SoundSample)));
  sound_sample_buffer->buffer = (SoundSample*) block;

  return OK;
}

void Platform_Main() {
  // TODO: relative, platform-agnostic filepath
  Platform_EngineLib* engine_lib;
  ASSERT_OK(Platform_CreateEngineLib(
        "Z:\\shellhack\\build\\engine\\Debug\\engine.dll",
        &engine_lib));

  Memory memory;
  ASSERT_OK(InitializeMemory(&memory));

  Platform_InputDevice* input_device;
  Input input;
  ASSERT_OK(InitializeInput(&input_device, &input));

  Platform_Window* window;
  PixelBuffer pixel_buffer;
  ASSERT_OK(InitializeGraphics(&window, &pixel_buffer));

  Platform_SoundDevice* sound_device;
  SoundSampleBuffer sound_sample_buffer;
  ASSERT_OK(InitializeAudio(&sound_device, &sound_sample_buffer));

  Platform_Stopwatch* frame_stopwatch;
  ASSERT_OK(Platform_CreateStopwatch(&frame_stopwatch));
  const float game_update_hz = (Platform_GetMonitorRefreshRate() / 2.0f);
  const float target_seconds_per_frame = 1.0f / game_update_hz;
  float dt_s = 0.0f;

  global_running = true;
  while(global_running) {
    ASSERT_OK(Platform_MaybeReloadEngineLib(engine_lib));

    Platform_HandleEvents();
    Platform_UpdateControllerStates(
        input_device,
        &input);

    // TODO: better error handling?
    ASSERT_OK((GetEngineUpdateFn(engine_lib))(
          dt_s,
          &memory,
          &input,
          &pixel_buffer,
          &sound_sample_buffer));

    // TODO: error handling?
    Platform_TryFillSoundBuffer(
        &sound_sample_buffer,
        sound_device);
    Platform_RenderBuffer(window, &pixel_buffer);

    Platform_UpdateStopwatch(&frame_stopwatch);
    float frame_seconds = Platform_SecondsElapsed(frame_stopwatch);
    if (frame_seconds < target_seconds_per_frame) {
      int32_t dur_ms = (int32_t)(1000.0f * (target_seconds_per_frame - frame_seconds));
      Platform_SleepMs(dur_ms);
    }

    Platform_UpdateStopwatch(&frame_stopwatch);
    dt_s = Platform_SecondsElapsed(frame_stopwatch);
    Platform_ResetStopwatch(&frame_stopwatch);

    // LOG_INFO("dt_s: %.04f", dt_s);
  }
}

void Platform_QuitGame() {
  global_running = false;
}
