#ifndef ENGINE
#define ENGINE

#include <common/status.h>
#include <platform/platform_runner.h>
#include <engine_export.h>

extern "C" ENGINE_EXPORT Status
Engine_Update(
	float dt_s,
	InputControllers* controllers,
	PixelBuffer* pixel_buffer,
	SoundSampleBuffer* sound_buffer);

#endif
