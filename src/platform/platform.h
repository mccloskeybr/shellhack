#ifndef PLATFORM
#define PLATFORM

#include <common/status.h>
#include <platform_main.h>
#include <inttypes.h>

//////////
// GRAPHICS
//////////

struct Platform_Window;

Status
Platform_CreateWindow(
	Platform_Window** window);

void
Platform_CloseWindow(
	Platform_Window* window);

Status
Platform_RenderBuffer(
	const Platform_Window* window,
	const PixelBuffer* const buffer);

//////////
// SOUND
//////////

struct Platform_SoundDevice;

Status
Platform_CreateSoundDevice(
	Platform_SoundDevice** device,
	const Platform_Window* const window,
	int32_t samples_per_second,
	int32_t bytes_per_sample,
	int32_t sound_buffer_sample_size);

void
Platform_CloseSoundDevice(Platform_SoundDevice* device);

// returns OK if buffer was successfully added to the queue.
// may return FAILED_PRECONDITION if the current committed queue is
// too high.
Status
Platform_TryFillSoundBuffer(
	const SoundSampleBuffer* const buffer,
	Platform_SoundDevice* device);

//////////
// INPUT
//////////

struct Platform_InputDevice;

Status
Platform_CreateInputDevice(
	Platform_InputDevice** device);

void
Platform_UpdateControllerStates(
	const Platform_InputDevice* const device,
	InputControllers* input_controllers);

//////////
// STOPWATCH
//////////

struct Platform_Stopwatch;

Status
Platform_CreateStopwatch(
	Platform_Stopwatch** stopwatch);

void
Platform_ResetStopwatch(
	Platform_Stopwatch** stopwatch);

void
Platform_UpdateStopwatch(
	Platform_Stopwatch** stopwatch);

float
Platform_SecondsElapsed(
	const Platform_Stopwatch* const stopwatch);

//////////
// HOTLOAD
//////////

#define ENGINE_UPDATE(name) Status name(   	 \
		InputControllers* controllers,   \
		PixelBuffer* pixel_buffer,       \
		SoundSampleBuffer* sound_buffer);
typedef ENGINE_UPDATE(EngineUpdateFn);

struct Platform_EngineLib;

Status
Platform_LoadEngineLib(
	Platform_EngineLib* engine_lib);

Status
Platform_ReleaseEngineLib(
	Platform_EngineLib* engine_lib);

//////////
// OS
//////////

void
Platform_HandleEvents();

float
Platform_GetMonitorRefreshRate();

void
Platform_SleepMs(
	int32_t duration_s);

#endif
