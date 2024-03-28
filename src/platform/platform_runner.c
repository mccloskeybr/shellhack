#include <platform/platform_runner.h>

#include <common/log.h>
#include <common/status.h>
#include <common/macros.h>
#include <engine/render/render.h>
#include <engine/input/input.h>
#include <engine/sound/sound.h>
#include <engine/memory/memory.h>
#include <platform/platform.h>
#include <math.h>

#include <windows.h>
#include <gl/gl.h>

static bool global_running;

static Status InitializeMemory(Memory* memory) {
  *memory = (Memory){};
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

static Status InitializeInput(
    struct Platform_InputDevice** input_device,
    Input* input) {
  RETURN_IF_ERROR(Platform_CreateInputDevice(input_device));
  return OK;
}

static Status InitializeAudio(
    struct Platform_SoundDevice** sound_device,
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

PlatformAPI CreatePlatformAPI() {
  PlatformAPI platform_api = {};
  platform_api.file_open_fn = Platform_FileOpen;
  platform_api.file_get_size_fn = Platform_FileGetSize;
  platform_api.file_read_fn = Platform_FileRead;
  platform_api.file_close_fn = Platform_FileClose;

  return platform_api;
}

void Platform_Main() {
  // TODO: relative, platform-agnostic filepath
  struct Platform_EngineLib* engine_lib;
  ASSERT_OK(Platform_CreateEngineLib(
        "Z:\\shellhack\\build\\engine\\Debug\\engine.dll",
        &engine_lib));

  Memory memory;
  ASSERT_OK(InitializeMemory(&memory));

  struct Platform_InputDevice* input_device;
  Input input;
  ASSERT_OK(InitializeInput(&input_device, &input));

  struct Platform_Graphics* graphics;
  ASSERT_OK(Platform_InitializeGraphics(&graphics));

  struct Platform_SoundDevice* sound_device;
  SoundSampleBuffer sound_sample_buffer;
  ASSERT_OK(InitializeAudio(&sound_device, &sound_sample_buffer));

  struct Platform_Stopwatch* frame_stopwatch;
  ASSERT_OK(Platform_CreateStopwatch(&frame_stopwatch));
  const float game_update_hz = (Platform_GetMonitorRefreshRate() / 2.0f);
  const float target_seconds_per_frame = 1.0f / game_update_hz;
  float dt_s = 0.0f;

  PlatformAPI platform_api = CreatePlatformAPI();

  global_running = true;
  while(global_running) {
    ASSERT_OK(Platform_MaybeReloadEngineLib(engine_lib));

    Platform_HandleEvents();
    Platform_UpdateControllerStates(
        input_device,
        &input);

    // TODO: better error handling?
    ASSERT_OK((Platform_GetEngineUpdateFn(engine_lib))(
          &platform_api,
          dt_s,
          &memory,
          &input,
          &sound_sample_buffer));

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(+0.5f, -0.5f);
    glVertex2f(+0.0f, +0.5f);
    glEnd();

    // TODO: error handling?
    Platform_TryFillSoundBuffer(
        &sound_sample_buffer,
        sound_device);
    Platform_SwapBuffer(graphics);

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
