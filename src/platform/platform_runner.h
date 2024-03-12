#ifndef ENGINE_MAIN
#define ENGINE_MAIN

#include <inttypes.h>

void
Platform_Main();

void
Platform_QuitGame();

//////////
// GRAPHICS
//////////

struct Pixel {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct PixelBuffer {
	Pixel* buffer;
	int32_t width;
	int32_t height;
};

//////////
// SOUND
//////////

struct SoundDescription {
	int32_t samples_per_second;
	int32_t bytes_per_sample;
};

struct SoundSample {
	int16_t l;
	int16_t r;
};

struct SoundSampleBuffer {
	SoundSample* buffer;
	int32_t buffer_size;
};

//////////
// INPUT
//////////

struct Stick {
	float x;
	float y;
};

struct Button {
	bool is_pressed;
};

struct Controller {
	bool is_connected;

	Stick stick;

	Button move_up;
	Button move_down;
	Button move_left;
	Button move_right;

	Button action_up;
	Button action_down;
	Button action_left;
	Button action_right;

	Button start;
	Button back;
};

struct InputControllers {
	Controller controllers[4];
};

//////////
// INPUT
//////////

struct Memory {
	void* permanent_storage;
	uint64_t permanent_storage_size;

	void* transient_storage;
	uint64_t transient_storage_size;
};

#endif
