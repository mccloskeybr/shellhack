#include <platform/platform.h>

#include <windows.h>

typedef struct Platform_Stopwatch {
  LARGE_INTEGER performance_frequency;
  LARGE_INTEGER start_time;
  LARGE_INTEGER last_tick;
} Platform_Stopwatch;

static inline LARGE_INTEGER Win32_GetWallClock() {
  LARGE_INTEGER performance_counter;
  QueryPerformanceCounter(&performance_counter);
  return performance_counter;
}

Status Platform_CreateStopwatch(Platform_Stopwatch** stopwatch) {
  *stopwatch = (Platform_Stopwatch*) malloc(
      sizeof(Platform_Stopwatch));
  if (*stopwatch == NULL) { return RESOURCE_EXHAUSTED; }

  QueryPerformanceFrequency(&(*stopwatch)->performance_frequency);
  Platform_ResetStopwatch(stopwatch);
  return OK;
}

void Platform_ResetStopwatch(Platform_Stopwatch** stopwatch) {
  const LARGE_INTEGER now = Win32_GetWallClock();
  (*stopwatch)->start_time = now;
  (*stopwatch)->last_tick = now;
}

void Platform_UpdateStopwatch(Platform_Stopwatch** stopwatch) {
  (*stopwatch)->last_tick = Win32_GetWallClock();
}

float Platform_SecondsElapsed(const Platform_Stopwatch* const stopwatch) {
  return ((float)(stopwatch->last_tick.QuadPart - stopwatch->start_time.QuadPart) /
      (float)stopwatch->performance_frequency.QuadPart);
}
