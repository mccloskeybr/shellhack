#include <platform/platform_runner.h>

#include <common/status.h>
#include <platform/platform_os.h>
#include <platform/platform_sound.h>
#include <platform/platform_input.h>
#include <platform/platform_graphics.h>
#include <platform/platform_stopwatch.h>
#include <platform/platform_hotload.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>

// TODO: remove platform-specific code
#include <windows.h>

#define NULL 0

static Platform_Window* global_window;
static Platform_SoundDevice* global_sound_device;
static Platform_InputDevice* global_input_device;
static Platform_MemoryManager* global_memory_manager;
static Platform_Stopwatch* global_frame_stopwatch;
static Platform_EngineLib* global_engine_lib;

static InputControllers* global_input_controllers;
static PixelBuffer* global_pixel_buffer;
static SoundSampleBuffer* global_sound_sample_buffer;

static bool global_running;
static float dt_s;

// TODO: put this in game memory?
static Status
InitializeInput(
	Platform_InputDevice** input_device,
	InputControllers* controllers) {

	RETURN_IF_ERROR(Platform_CreateInputDevice(input_device));
	ASSERT(*input_device != NULL);

	global_input_controllers = (InputControllers*) malloc(
		sizeof(InputControllers));
	if(global_input_controllers == NULL) { return RESOURCE_EXHAUSTED; }

	return OK;
}

// TODO: put this in game memory?
static Status
InitializeGraphics(
	Platform_Window** window,
	PixelBuffer* pixel_buffer) {

	RETURN_IF_ERROR(Platform_CreateWindow(&global_window));
	ASSERT(global_window != NULL);

	global_pixel_buffer = (PixelBuffer*) malloc(sizeof(PixelBuffer));
	if (global_pixel_buffer == NULL) { return RESOURCE_EXHAUSTED; }
	global_pixel_buffer->width = 1280;
	global_pixel_buffer->height = 720;
	global_pixel_buffer->buffer = (Pixel*) malloc(
		global_pixel_buffer->width * global_pixel_buffer->height * sizeof(Pixel));

	return OK;
}

// TODO: put this in game memory?
static Status
InitializeAudio() {
	RETURN_IF_ERROR(Platform_CreateSoundDevice(
		&global_sound_device,
		48000,
		sizeof(int16_t) * 2,
		48000));
	ASSERT(global_sound_device != NULL);

	global_sound_sample_buffer = (SoundSampleBuffer*) malloc(
		sizeof(SoundSampleBuffer));
	if (global_sound_sample_buffer == NULL) { return RESOURCE_EXHAUSTED; }
	global_sound_sample_buffer->buffer_size = 48000 / 30;
	global_sound_sample_buffer->buffer = (SoundSample*) malloc(
		global_sound_sample_buffer->buffer_size * sizeof(SoundSample));
	if (global_sound_sample_buffer->buffer == NULL) { return RESOURCE_EXHAUSTED; }

	return OK;
}

static Status
InitializeMemory() {
	// NOW
	// TODO: fill this out.
	RETURN_IF_ERROR(Platform_InitializeMemory(global_memory_manager

				
}

static void
PrintDt() {
	char fps_buffer[256];
	sprintf_s(fps_buffer, "%.04f sec per frame\n", dt_s);
	OutputDebugStringA(fps_buffer);
}

void
Platform_Main() {
	// TODO: relative filepath
	ASSERT_OK(Platform_CreateEngineLib(
			"Z:\\shellhack\\build\\engine\\Debug\\engine.dll",
			&global_engine_lib));

	// NOW
	// TODO: no more globals
	Platform_Window* window;
	PixelBuffer pixel_buffer;
	ASSERT_OK(InitializeGraphics(&window, &pixel_buffer));

	ASSERT_OK(InitializeAudio());
	ASSERT_OK(InitializeInput());

	ASSERT_OK(Platform_CreateStopwatch(&global_frame_stopwatch));
	const float game_update_hz = (Platform_GetMonitorRefreshRate() / 2.0f);
	const float target_seconds_per_frame = 1.0f / game_update_hz;

	global_running = true;
	while(global_running) {
		ASSERT_OK(Platform_MaybeReloadEngineLib(global_engine_lib));

		Platform_HandleEvents();
		Platform_UpdateControllerStates(
				global_input_device,
				global_input_controllers);

		// TODO: better error handling?
		ASSERT_OK((GetEngineUpdateFn(global_engine_lib))(
				dt_s,
				global_input_controllers,
				global_pixel_buffer,
				global_sound_sample_buffer));

		// TODO: error handling?
		Platform_TryFillSoundBuffer(
			global_sound_sample_buffer,
			global_sound_device);
		Platform_RenderBuffer(global_window, global_pixel_buffer);

		Platform_UpdateStopwatch(&global_frame_stopwatch);
		float frame_seconds = Platform_SecondsElapsed(global_frame_stopwatch);
		if (frame_seconds < target_seconds_per_frame) {
			int32_t dur_ms = (int32_t)(1000.0f * (target_seconds_per_frame - frame_seconds));
			Platform_SleepMs(dur_ms);
		}

		Platform_UpdateStopwatch(&global_frame_stopwatch);
		dt_s = Platform_SecondsElapsed(global_frame_stopwatch);
		PrintDt();
		Platform_ResetStopwatch(&global_frame_stopwatch);
	}
}

void Platform_QuitGame() {
	global_running = false;
}
