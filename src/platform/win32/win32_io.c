#include <platform/platform.h>

#include <common/status.h>
#include <common/macros.h>
#include <windows.h>

typedef struct Platform_FileHandle {
  HANDLE handle;
} Platform_FileHandle;

PLATFORM_FILE_OPEN(Platform_FileOpen) {
  *file_handle = (Platform_FileHandle*) VirtualAlloc(
      0, sizeof(Platform_FileHandle), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  if (*file_handle == NULL) { return RESOURCE_EXHAUSTED; }

  (*file_handle)->handle =
    CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
  ASSERT((*file_handle)->handle != INVALID_HANDLE_VALUE);

  return OK;
}

PLATFORM_FILE_GET_SIZE(Platform_FileGetSize) {
  ASSERT(file_handle != NULL);
  ASSERT(file_handle->handle != INVALID_HANDLE_VALUE);

  return (int32_t) GetFileSize(file_handle->handle, NULL);
}

PLATFORM_FILE_READ(Platform_FileRead) {
  ASSERT(file_handle != NULL);
  ASSERT(file_handle->handle != INVALID_HANDLE_VALUE);
  ASSERT(size <= Platform_FileGetSize(file_handle));

  DWORD bytes_read;
  BOOL success = ReadFile(file_handle->handle, dest, size, &bytes_read, NULL);
  if (!success) { return INVALID_ARGUMENT; }
  else { return OK; }
}

PLATFORM_FILE_CLOSE(Platform_FileClose) {
  if (file_handle->handle != INVALID_HANDLE_VALUE) {
    CloseHandle(file_handle->handle);
  }
  VirtualFree(file_handle, 0, MEM_RELEASE);
}
