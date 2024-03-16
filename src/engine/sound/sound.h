#ifndef ENGINE_SOUND
#define ENGINE_SOUND

#include <inttypes.h>

struct SoundSample {
  int16_t l;
  int16_t r;
};

struct SoundSampleBuffer {
  SoundSample* buffer;
  int32_t buffer_size;
};

#endif
