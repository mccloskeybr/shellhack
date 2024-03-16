#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include <stdio.h>
#include <stdarg.h>

#define LOG(level, fmt, ...) Log_OutputMsg(level, fmt, ##__VA_ARGS__);
#define LOG_INFO(fmt, ...) LOG(LOG_INFO, fmt, ##__VA_ARGS__);
#define LOG_ERROR(fmt, ...) LOG(LOG_ERROR, fmt, ##__VA_ARGS__);

enum LogLevel {
  LOG_INFO,
  LOG_ERROR
};

void Log_OutputMsg(
    const LogLevel level,
    const char* const fmt,
    ...);

#endif
