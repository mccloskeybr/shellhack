#include <engine/engine.h>

#include <common/status.h>
#include <platform/platform_runner.h>
#include <engine_export.h>
#include <malloc.h>
#include <math.h>

static void
UpdatePixelBuff(
	PixelBuffer* pixel_buffer) {

	const int32_t pitch = pixel_buffer->width * sizeof(Pixel);
	uint8_t* row = (uint8_t*) pixel_buffer->buffer;
	for (int y = 0; y < pixel_buffer->height; y++) {
		Pixel* pixel = (Pixel*) row;
		for (int x = 0; x < pixel_buffer->width; x++) {
			pixel->r = x;
			pixel->g = 0;
			pixel->b = y;
			pixel++;
		}
		row += pitch;
	}
}

static void
UpdateSoundBuff(
	SoundSampleBuffer* sound_buffer) {

    	static float t_sine = 0.0f;
    	int tone_hz = 256;
    	int16_t tone_vol = 3000;
    	int wave_period = 48000 / tone_hz;

	for (int i = 0; i < sound_buffer->buffer_size; i++) {
		float sine_val = sinf(t_sine);
		int16_t sample_val = (int16_t)(sine_val * tone_vol);
		sound_buffer->buffer[i].l = sample_val;
		sound_buffer->buffer[i].r = sample_val;

		t_sine += 2.0f*3.14f*1.0f/(float)wave_period;
	}
}

extern "C" ENGINE_EXPORT Status
Engine_Update(
	float dt_s,
	InputControllers* controllers,
	PixelBuffer* pixel_buffer,
	SoundSampleBuffer* sound_buffer) {

	UpdatePixelBuff(pixel_buffer);
	UpdateSoundBuff(sound_buffer);

	return OK;
}
