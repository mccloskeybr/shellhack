#ifndef PLATFORM_OS
#define PLATFORM_OS

#include <platform/platform_runner.h>
#include <common/status.h>
#include <inttypes.h>

void*
Platform_InitializeMemory(
	size_t size);

void
Platform_HandleEvents();

float
Platform_GetMonitorRefreshRate();

void
Platform_SleepMs(
	int32_t duration_s);

#endif
