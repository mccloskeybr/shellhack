#include <platform/platform_hotload.h>

#include <common/status.h>
#include <engine/render/render.h>
#include <engine/input/input.h>
#include <engine/sound/sound.h>
#include <engine/memory/memory.h>
#include <windows.h>

struct Platform_EngineLib {
  char lib_file_path[256];
  char lib_lock_file_path[256];
  FILETIME lib_write_time;

  HMODULE lib;
  EngineUpdateFn* update_fn;
};

static const uint32_t N_LOCK_RETRY = 30;
static const uint32_t LOCK_WAIT_MS = 10;

static inline Status
Win32_GetLastWriteTime(
    char* file_path,
    FILETIME* filetime) {
  WIN32_FILE_ATTRIBUTE_DATA file_data;
  int retry = 0;
  if (!GetFileAttributesEx(
        file_path,
        GetFileExInfoStandard,
        &file_data)) {
    return INVALID_ARGUMENT;
  }

  *filetime = file_data.ftLastWriteTime;
  return OK;
}


static Status
Win32_LoadEngineLib(Platform_EngineLib* engine_lib) {
  if(engine_lib->lib != NULL) {
    return OK;
  }
  ASSERT(engine_lib->update_fn == NULL);

  bool copied_file = false;
  for (uint32_t retry = 0; retry < N_LOCK_RETRY; retry++) {
    if (CopyFile(
          engine_lib->lib_file_path,
          engine_lib->lib_lock_file_path,
          FALSE)) {
      copied_file = true;
      break;
    }
    Sleep(LOCK_WAIT_MS);
  }
  if (!copied_file) {
    // file doesn't exist or couldn't get lock.
    return INVALID_ARGUMENT;
  }

  FILETIME write_time;
  RETURN_IF_ERROR(
      Win32_GetLastWriteTime(engine_lib->lib_file_path, &write_time));

  const HMODULE lib = LoadLibraryA(engine_lib->lib_lock_file_path);
  if (lib == NULL) { return INTERNAL; }

  EngineUpdateFn* update_fn =
    (EngineUpdateFn*) GetProcAddress(lib, "Engine_Update");
  if (update_fn == NULL) { return INTERNAL; }

  engine_lib->lib = lib;
  engine_lib->update_fn = update_fn;
  engine_lib->lib_write_time = write_time;

  return OK;
}

static Status
Win32_ReleaseEngineLib(Platform_EngineLib* engine_lib) {
  if (engine_lib->lib == NULL) {
    return OK;
  }
  ASSERT(engine_lib->update_fn != NULL);

  FreeLibrary(engine_lib->lib);
  engine_lib->lib = NULL;
  engine_lib->update_fn = NULL;
  engine_lib->lib_write_time = {};

  return OK;
}

Status
Platform_MaybeReloadEngineLib(Platform_EngineLib* engine_lib) {
  FILETIME last_write_time;
  RETURN_IF_ERROR(Win32_GetLastWriteTime(engine_lib->lib_file_path, &last_write_time));

  const bool is_new_write = CompareFileTime(&engine_lib->lib_write_time, &last_write_time) != 0;
  if (!is_new_write) { return OK; }

  RETURN_IF_ERROR(Win32_ReleaseEngineLib(engine_lib));
  RETURN_IF_ERROR(Win32_LoadEngineLib(engine_lib));
  return OK;
}

// TODO: allow lib_path to not be a full path.
// TODO: remove reliance on MAX_PATH.
Status
Platform_CreateEngineLib(
    char* lib_file_path,
    Platform_EngineLib** engine_lib) {

  *engine_lib = (Platform_EngineLib*) calloc(1, sizeof(Platform_EngineLib));
  if (*engine_lib == NULL) { return RESOURCE_EXHAUSTED; }

  char* ext_ptr = lib_file_path;
  while (*ext_ptr != '.') { ext_ptr++; }
  const int32_t ext_idx = ext_ptr - lib_file_path;

  char lib_stem[MAX_PATH];
  strncpy_s(lib_stem, lib_file_path, ext_idx);

  char lib_ext[MAX_PATH];
  strcpy_s(lib_ext, ext_ptr);

  const char* const locked_suffix = "_LOCKED";

  char lib_lock_file_path[MAX_PATH] = {'\0'};
  strcat_s(lib_lock_file_path, lib_stem);
  strcat_s(lib_lock_file_path, locked_suffix);
  strcat_s(lib_lock_file_path, lib_ext);

  strcpy_s((*engine_lib)->lib_file_path, lib_file_path);
  strcpy_s((*engine_lib)->lib_lock_file_path, lib_lock_file_path);

  RETURN_IF_ERROR(Win32_LoadEngineLib(*engine_lib));
  return OK;
}

EngineUpdateFn*
GetEngineUpdateFn(Platform_EngineLib* engine_lib) {
  return engine_lib->update_fn;
}
