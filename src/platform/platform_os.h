#ifndef PLATFORM_OS
#define PLATFORM_OS

#include <common/status.h>
#include <platform/platform_runner.h>
#include <inttypes.h>

Status Platform_InitializeMemory(
    void** block,
    uint64_t size);

void Platform_HandleEvents();

float Platform_GetMonitorRefreshRate();

void Platform_SleepMs(int32_t duration_s);

#endif
