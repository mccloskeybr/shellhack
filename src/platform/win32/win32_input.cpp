#include <platform/platform_input.h>

#include <platform/platform_runner.h>
#include <common/macros.h>
#include <common/status.h>
#include <inttypes.h>
#include <windows.h>
#include <xinput.h>

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState);
typedef X_INPUT_GET_STATE(XInputGetStateFn);

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
typedef X_INPUT_SET_STATE(XInputSetStateFn);

struct Platform_InputDevice {
	XInputGetStateFn* x_input_get_state_fn;
	XInputSetStateFn* x_input_set_state_fn;
};

static inline Status
GetXInputLibrary(HMODULE* lib) {
	*lib = LoadLibraryA("xinput1_4.dll");
	if (*lib != NULL) { return OK; }

	*lib = LoadLibraryA("xinput9_1_0.dll");
	if (*lib != NULL) { return OK; }

	*lib = LoadLibraryA("xinput1_3.dll");
	if (*lib != NULL) { return OK; }

	return INTERNAL;
}

static inline Controller*
GetController(
	InputControllers* input_controllers,
	uint32_t index) {

	ASSERT(index < ARRAY_SIZE(input_controllers->controllers));
	return &input_controllers->controllers[index];
}

static inline bool
IsButtonPressed(
	DWORD x_input_button_state,
	DWORD button_bit) {

	return (x_input_button_state & button_bit) == button_bit;
}

// TODO: square deadzone currently
static inline float
StickValue(SHORT stick_value) {
	static const SHORT DEADZONE = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	float result = 0;
	if (stick_value < -DEADZONE) {
		result = (float)((stick_value + DEADZONE) / (32768.0f - DEADZONE));
	} else if (stick_value > DEADZONE) {
		result = (float)((stick_value - DEADZONE) / (32767.0f - DEADZONE));
	}
	return result;
}

static Status
Win32_LoadXInputFunctions(
	XInputGetStateFn** x_input_get_state_fn,
	XInputSetStateFn** x_input_set_state_fn) {

	HMODULE x_input_lib;
	RETURN_IF_ERROR(GetXInputLibrary(&x_input_lib));

	*x_input_get_state_fn = (XInputGetStateFn*) GetProcAddress(
			x_input_lib, "XInputGetState");
	if (*x_input_get_state_fn == NULL) { return INTERNAL; }

	*x_input_set_state_fn = (XInputSetStateFn*) GetProcAddress(
			x_input_lib, "XInputSetState");
	if (*x_input_get_state_fn == NULL) { return INTERNAL; }

	return OK;
}

static Status
Win32_UpdateControllerStates(
	XInputGetStateFn* x_input_get_state_fn,
	XInputSetStateFn* x_input_set_state_fn,
	InputControllers* input_controllers) {

	// TODO: index 0 assumed to be keyboard right now, should prob. be dynamic.
	// TODO: skip disconnected controllers
	for (DWORD i = 1; i < 4; i++) {
		Controller* c = GetController(input_controllers, i);

		XINPUT_STATE x_input_state;
		// subtract 1 to correct for keyboard -> 0 assumption.
		if (x_input_get_state_fn(i - 1, &x_input_state) != ERROR_SUCCESS) {
			c->is_connected = false;
			continue;
		}
		XINPUT_GAMEPAD* gamepad = &x_input_state.Gamepad;

		c->is_connected = true;

		c->stick.x = StickValue(gamepad->sThumbLX);
		c->stick.y = StickValue(gamepad->sThumbLY);

		const DWORD bstate = gamepad->wButtons;
		c->move_up.is_pressed = IsButtonPressed(bstate, XINPUT_GAMEPAD_DPAD_UP);
		c->move_down.is_pressed = IsButtonPressed(bstate, XINPUT_GAMEPAD_DPAD_DOWN);
		c->move_left.is_pressed = IsButtonPressed(bstate, XINPUT_GAMEPAD_DPAD_LEFT);
		c->move_right.is_pressed = IsButtonPressed(bstate, XINPUT_GAMEPAD_DPAD_RIGHT);

		c->action_up.is_pressed = IsButtonPressed(bstate, XINPUT_GAMEPAD_Y);
		c->action_down.is_pressed = IsButtonPressed(bstate, XINPUT_GAMEPAD_A);
		c->action_left.is_pressed = IsButtonPressed(bstate, XINPUT_GAMEPAD_X);
		c->action_right.is_pressed = IsButtonPressed(bstate, XINPUT_GAMEPAD_B);

		c->start.is_pressed = IsButtonPressed(bstate, XINPUT_GAMEPAD_START);
		c->back.is_pressed = IsButtonPressed(bstate, XINPUT_GAMEPAD_BACK);
	}

	return OK;
}

Status
Platform_CreateInputDevice(
	Platform_InputDevice** device) {

	*device = (Platform_InputDevice*) malloc(
			sizeof(Platform_InputDevice));
	if (*device == NULL) { return RESOURCE_EXHAUSTED; }

	Status load_lib_status = Win32_LoadXInputFunctions(
					&(*device)->x_input_get_state_fn,
					&(*device)->x_input_set_state_fn);
	if (load_lib_status != OK) {
		free(*device);
		return INTERNAL;
	}

	return OK;
}

void
Platform_UpdateControllerStates(
	const Platform_InputDevice* const device,
	InputControllers* input_controllers) {

	Win32_UpdateControllerStates(
			*device->x_input_get_state_fn,
			*device->x_input_set_state_fn,
			input_controllers);
}
