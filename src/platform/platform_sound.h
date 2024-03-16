#ifndef PLATFORM_SOUND
#define PLATFORM_SOUND

#include <common/status.h>
#include <engine/sound/sound.h>
#include <inttypes.h>

struct Platform_SoundDevice;

Status
Platform_CreateSoundDevice(
    Platform_SoundDevice** device,
    int32_t samples_per_second,
    int32_t bytes_per_sample,
    int32_t sound_buffer_sample_size);

Status
Platform_TryFillSoundBuffer(
    const SoundSampleBuffer* const samples,
    Platform_SoundDevice* device);

#endif
