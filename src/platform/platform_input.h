#ifndef PLATFORM_INPUT
#define PLATFORM_INPUT

#include <common/status.h>
#include <platform/platform_runner.h>

struct Platform_InputDevice;

Status
Platform_CreateInputDevice(
	Platform_InputDevice** device);

void
Platform_UpdateControllerStates(
	const Platform_InputDevice* const device,
	InputControllers* input_controllers);

#endif
