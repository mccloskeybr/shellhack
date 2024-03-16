#ifndef PLATFORM_STOPWATCH
#define PLATFORM_STOPWATCH

#include <common/status.h>

struct Platform_Stopwatch;

Status Platform_CreateStopwatch(Platform_Stopwatch** stopwatch);

void Platform_CloseStopwatch(Platform_Stopwatch* stopwatch);

void Platform_ResetStopwatch(Platform_Stopwatch** stopwatch);

void Platform_UpdateStopwatch(Platform_Stopwatch** stopwatch);

float Platform_SecondsElapsed(const Platform_Stopwatch* const stopwatch);

#endif
