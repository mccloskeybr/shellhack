#ifndef ENGINE_RENDER_ASSET_SERVER_H
#define ENGINE_RENDER_ASSET_SERVER_H

#include <common/status.h>
#include <engine/memory/allocator.h>
#include <inttypes.h>

typedef uint32_t Asset_Handle;

typedef enum Asset_Type {
  AT_UNDEF,
  AT_SHADER
} Asset_Type;

typedef struct Asset_Header {
  Asset_Type type;
  Asset_Handle handle;

  // also include last load time? for hotloading.
  // when hitting a load call, can check if the latest write time is newer than
  // the saved time.

  struct Asset_Header* next;
} Asset_Header;

typedef struct Asset_Shader {
  Asset_Header header;
  char* shader_code;
} Asset_Shader;

typedef struct Asset_Server {
  Allocator allocator;
  struct PlatformAPI* platform_api;
  Asset_Header sentinel;
} Asset_Server;

void Asset_ServerInitialize(Asset_Server* asset_server,
                            struct PlatformAPI* platform_api,
                            Arena asset_server_arena);
Status Asset_ServerLoadAsset(Asset_Server* server,
                             Asset_Type type,
                             char* asset_path,
                             Asset_Handle* handle);
bool Asset_ServerGetAsset(Asset_Server* server,
                          Asset_Handle handle,
                          Asset_Header** asset_header);
Status Asset_ServerFreeAsset(Asset_Server* server,
                             Asset_Handle handle);

#endif
