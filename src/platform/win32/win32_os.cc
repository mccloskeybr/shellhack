#include <platform/platform_os.h>

#include <common/status.h>
#include <common/macros.h>
#include <platform/platform_runner.h>
#include <windows.h>

Status Platform_InitializeMemory(
    void** block,
    uint64_t size) {
  *block = VirtualAlloc(0, (size_t) size,
      MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  if (*block == NULL) { return RESOURCE_EXHAUSTED; }

  return OK;
}

void Platform_HandleEvents() {
  MSG message;
  while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
    if (message.message == WM_QUIT) {
      Platform_QuitGame();
    }
    TranslateMessage(&message);
    DispatchMessageA(&message);
  }
}

float Platform_GetMonitorRefreshRate() {
  // TODO: programmatically determine this.
  return 60.0;
}

void Platform_SleepMs(int32_t duration_ms) {
  if (duration_ms < 0) { return; }
  Sleep(duration_ms);
}
