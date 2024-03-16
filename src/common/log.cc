#include <common/log.h>

#include <stdio.h>
#include <stdarg.h>

// TODO: make platform agnostic
#include <windows.h>

void Log_OutputMsg(
    const LogLevel level,
    const char* const fmt,
    ...) {


  switch (level) {
    case LOG_INFO: {
      OutputDebugStringA("[INFO] ");
    } break;
    case LOG_ERROR: {
      OutputDebugStringA("[ERROR] ");
    } break;
  }

  char str[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(str, sizeof(str), fmt, args);
  OutputDebugStringA(str);
  OutputDebugStringA("\n");
}
