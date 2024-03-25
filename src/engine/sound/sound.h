#ifndef ENGINE_SOUND
#define ENGINE_SOUND

#include <inttypes.h>

typedef struct SoundSample {
  int16_t l;
  int16_t r;
} SoundSample;

typedef struct SoundSampleBuffer {
  SoundSample* buffer;
  int32_t buffer_size;
} SoundSampleBuffer;

#endif
