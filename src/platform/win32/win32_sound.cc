#include <platform/platform_sound.h>

#include <common/log.h>
#include <common/status.h>
#include <dsound.h>
#include <engine/sound/sound.h>
#include <inttypes.h>
#include <windows.h>

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(DirectSoundCreateFn);

struct Platform_SoundDevice {
  int32_t samples_per_second;
  int32_t bytes_per_sample;

  LPDIRECTSOUNDBUFFER sound_buffer;
  int32_t sound_buffer_sample_size;

  DWORD sample_fill_cursor;
};

static const int32_t TOLERANCE = 4800;

static WAVEFORMATEX
Win32_GetWaveFormat(int32_t samples_per_second) {
  WAVEFORMATEX format = {};
  format.wFormatTag = WAVE_FORMAT_PCM;
  format.nChannels = 2;
  format.nSamplesPerSec = samples_per_second;
  format.wBitsPerSample = 16;
  format.nBlockAlign = (format.nChannels * format.wBitsPerSample) / 8;
  format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
  format.cbSize = 0;
  return format;
}

static Status
Win32_InitDirectSound(
    LPDIRECTSOUND* direct_sound) {
  HMODULE d_sound_library = LoadLibraryA("dsound.dll");
  if (d_sound_library == NULL) {
    // log
    return INTERNAL;
  }

  DirectSoundCreateFn* direct_sound_create_fn =
    (DirectSoundCreateFn*) GetProcAddress(d_sound_library, "DirectSoundCreate");
  if (direct_sound_create_fn == NULL) {
    // log
    return INTERNAL;
  }

  if (!SUCCEEDED(direct_sound_create_fn(0, direct_sound, 0))) {
    // log
    return INTERNAL;
  }

  const HWND window = FindWindowA("GameWindowClass", 0);
  if (!SUCCEEDED((*direct_sound)->SetCooperativeLevel(window, DSSCL_PRIORITY))) {
    // log
    return INTERNAL;
  }

  LOG_INFO("Direct Sound initialized successfully.");
  return OK;
}

static Status
Win32_FormatPrimarySoundBuffer(
    const LPDIRECTSOUND* const direct_sound,
    int32_t samples_per_second) {
  DSBUFFERDESC description = {};
  description.dwSize = sizeof(description);
  description.dwFlags = DSBCAPS_PRIMARYBUFFER;

  LPDIRECTSOUNDBUFFER buffer;
  if (!SUCCEEDED((*direct_sound)->CreateSoundBuffer(&description, &buffer, 0))) {
    // log
    return INTERNAL;
  }

  WAVEFORMATEX format = Win32_GetWaveFormat(samples_per_second);
  if (!SUCCEEDED(buffer->SetFormat(&format))) {
    // log
    return INTERNAL;
  }

  LOG_INFO("Direct Sound Primary Buffer set.");
  return OK;
}

static Status
Win32_InitSecondarySoundBuffer(
    const LPDIRECTSOUND* const direct_sound,
    int32_t samples_per_second,
    int32_t bytes_per_sample,
    LPDIRECTSOUNDBUFFER* secondary_buffer,
    int32_t sound_buffer_sample_size) {
  WAVEFORMATEX format = Win32_GetWaveFormat(samples_per_second);

  DSBUFFERDESC description = {};
  description.dwSize = sizeof(description);
  description.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
  description.dwBufferBytes = sound_buffer_sample_size * bytes_per_sample;
  description.lpwfxFormat = &format;

  if (!SUCCEEDED((*direct_sound)->CreateSoundBuffer(&description, secondary_buffer, 0))) {
    // log
    return INTERNAL;
  }

  LOG_INFO("Direct Sound Secondary Buffer set.");
  return OK;
}

static Status
Win32_ClearSoundBuffer(LPDIRECTSOUNDBUFFER* buffer) {
  VOID* region_1;
  DWORD region_1_size;
  VOID* region_2;
  DWORD region_2_size;

  HRESULT result = (*buffer)->Lock(0, 0,
      &region_1, &region_1_size,
      &region_2, &region_2_size,
      DSBLOCK_ENTIREBUFFER);
  if(!SUCCEEDED(result)) {
    LOG_ERROR("Unable to clear secondary buffer!");
    return INTERNAL;
  }
  uint8_t* sample;

  sample = (uint8_t*) region_1;
  for(DWORD i = 0; i < region_1_size; i++) {
    *sample++ = 0;
  }

  sample = (uint8_t*) region_2;
  for(DWORD i = 0; i < region_2_size; i++) {
    *sample++ = 0;
  }

  (*buffer)->Unlock(region_1, region_1_size, region_2, region_2_size);
  return OK;
}

// TODO: audio starts to skip when the provided buffer is too big.
// probably solvable with more edge case handling?
// TODO: proper error handling
// TODO: investigate audio skips at launch
static Status
Win32_TryFillSoundBuffer(
    const SoundSampleBuffer* const samples,
    int32_t bytes_per_sample,
    LPDIRECTSOUNDBUFFER* sound_buffer,
    int32_t sound_buffer_sample_size,
    DWORD* sample_fill_cursor) {
  // TODO: investigate GetCaps() instead of passing sample size around.

  const int32_t sample_buffer_byte_size = samples->buffer_size * bytes_per_sample;
  const int32_t sound_buffer_byte_size = sound_buffer_sample_size * bytes_per_sample;
  if (sample_buffer_byte_size >= sound_buffer_byte_size) {
    LOG_ERROR("Provided sample buffer exceeds configured limit!");
    return INVALID_ARGUMENT;
  }

  DWORD play_cursor_unsigned;
  DWORD write_cursor_unsigned;
  if(!SUCCEEDED((*sound_buffer)->GetCurrentPosition(
          &play_cursor_unsigned,
          &write_cursor_unsigned))) {
    LOG_ERROR("Unable to get sample cursor positions!");
    return INTERNAL;
  }
  int32_t play_cursor = (int32_t) play_cursor_unsigned;
  int32_t write_cursor = (int32_t) write_cursor_unsigned;

  // NOTE: Tolerance required because play_cursor can be within +- 15ms.
  // In the worst case, we would allow audio to be pulled right before it's queued.
  // TODO: find a better value for tolerance.
  play_cursor -= TOLERANCE;
  if (play_cursor < 0) {
    play_cursor += sound_buffer_byte_size;
  }

  int32_t start_cursor = (int32_t) *sample_fill_cursor;
  int32_t end_cursor = (start_cursor + sample_buffer_byte_size) % sound_buffer_byte_size;

  // NOTE: audio skips when checking for unwrapped end cursors?
  if (play_cursor < start_cursor && start_cursor < write_cursor) {
    // TODO: the start cursor should sync to write in this case?
    // (barring the case where start is a full-buffers length ahead of play)
    return FAILED_PRECONDITION;
  }
  if (play_cursor < end_cursor && end_cursor < write_cursor) {
    return FAILED_PRECONDITION;
  }
  if (start_cursor < play_cursor && write_cursor < end_cursor) {
    return FAILED_PRECONDITION;
  }

  VOID* region_1;
  DWORD region_1_size;
  VOID* region_2;
  DWORD region_2_size;
  if(SUCCEEDED((*sound_buffer)->Lock(start_cursor, sample_buffer_byte_size,
          &region_1, &region_1_size,
          &region_2, &region_2_size,
          0))) {
    int16_t* dest_sample;

    DWORD n_samples_region_1 = region_1_size / bytes_per_sample;
    dest_sample = (int16_t*) region_1;
    for(DWORD i = 0; i < n_samples_region_1; i++) {
      *dest_sample++ = samples->buffer[i].l;
      *dest_sample++ = samples->buffer[i].r;

      *sample_fill_cursor += bytes_per_sample;
      *sample_fill_cursor %= sound_buffer_byte_size;
    }

    DWORD n_samples_region_2 = region_2_size / bytes_per_sample;
    dest_sample = (int16_t*) region_2;
    for(DWORD i = 0; i < n_samples_region_2; i++) {
      *dest_sample++ = samples->buffer[n_samples_region_1 + i].l;
      *dest_sample++ = samples->buffer[n_samples_region_1 + i].r;

      *sample_fill_cursor += bytes_per_sample;
      *sample_fill_cursor %= sound_buffer_byte_size;
    }

    (*sound_buffer)->Unlock(region_1, region_1_size, region_2, region_2_size);
    return OK;
  } else {
    LOG_ERROR("Could not retrieve lock on sound buffer!");
    return INTERNAL;
  }
}

Status
Platform_CreateSoundDevice(
    Platform_SoundDevice** device,
    int32_t samples_per_second,
    int32_t bytes_per_sample,
    int32_t sound_buffer_sample_size) {

  LPDIRECTSOUND direct_sound;
  RETURN_IF_ERROR(Win32_InitDirectSound(&direct_sound));
  RETURN_IF_ERROR(Win32_FormatPrimarySoundBuffer(
        &direct_sound,
        samples_per_second));

  LPDIRECTSOUNDBUFFER secondary_buffer;
  RETURN_IF_ERROR(Win32_InitSecondarySoundBuffer(
        &direct_sound,
        samples_per_second,
        bytes_per_sample,
        &secondary_buffer,
        sound_buffer_sample_size));
  if (!SUCCEEDED(secondary_buffer->Play(0, 0, DSBPLAY_LOOPING))) {
    return INTERNAL;
  }

  *device = (Platform_SoundDevice*) malloc(sizeof(Platform_SoundDevice));
  if (*device == NULL) { return RESOURCE_EXHAUSTED; }
  (*device)->sound_buffer = secondary_buffer;
  (*device)->sample_fill_cursor = 0;
  (*device)->sound_buffer_sample_size = sound_buffer_sample_size;
  (*device)->samples_per_second = samples_per_second;
  (*device)->bytes_per_sample = bytes_per_sample;

  RETURN_IF_ERROR(Win32_ClearSoundBuffer(&((*device)->sound_buffer)));
  LOG_INFO("Sound device created successfully.");
  return OK;
}

Status
Platform_TryFillSoundBuffer(
    const SoundSampleBuffer* const samples,
    Platform_SoundDevice* device) {

  return Win32_TryFillSoundBuffer(samples,
      device->bytes_per_sample,
      &device->sound_buffer,
      device->sound_buffer_sample_size,
      &device->sample_fill_cursor);
}
