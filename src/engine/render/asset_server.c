#include <engine/render/asset_server.h>

#include <common/status.h>
#include <engine/memory/memory.h>
#include <engine/memory/allocator.h>
#include <platform/platform.h>

static Asset_Handle Asset_HandleCreate(char* asset_path) {
  uint32_t hash = 5381;
  uint8_t* c = asset_path;
  while (*c != '\0') {
    hash = ((hash << 5) + hash) + *c;
    c++;
  }
  return hash;
}

void Asset_ServerInitialize(Asset_Server* asset_server,
                            struct PlatformAPI* platform_api,
                            Arena asset_server_arena) {
  *asset_server = (Asset_Server){};
  ASSERT_OK(Allocator_Init(&asset_server->allocator, asset_server_arena));
  asset_server->platform_api = platform_api;
  asset_server->sentinel.next = &asset_server->sentinel;
}

Status Asset_ServerLoadAsset(Asset_Server* server,
                             Asset_Type type,
                             char* asset_path,
                             Asset_Handle* handle) {
  ASSERT(server != NULL);
  ASSERT(type != AT_UNDEF);

  // check if the asset has already been loaded
  *handle = Asset_HandleCreate(asset_path);
  Asset_Header* ignored;
  if (Asset_ServerGetAsset(server, *handle, &ignored)) { return OK; }

  // allocate the asset
  Asset_Header* header = NULL;
  switch(type) {
    case AT_SHADER: {
      Asset_Shader* shader = (Asset_Shader*) Allocator_AllocateBlock(
          &server->allocator, sizeof(Asset_Shader));
      header = &shader->header;
    } break;
    default: {
      return INVALID_ARGUMENT;
    } break;
  }
  ASSERT(header != NULL);

  // TODO: deallocate on error
  struct Platform_FileHandle* file_handle;
  // TODO: graceful error?
  RETURN_IF_ERROR(server->platform_api->file_open_fn(
        &file_handle, asset_path));

  header->type = type;
  header->handle = *handle;
  header->next = server->sentinel.next;
  server->sentinel.next = header;

  int32_t file_size = server->platform_api->file_get_size_fn(file_handle);
  void* file_data = Allocator_AllocateBlock(&server->allocator, file_size);
  // TODO: deallocate on error
  RETURN_IF_ERROR(server->platform_api->file_read_fn(
      file_handle, file_data, file_size));

  switch (header->type) {
    case AT_SHADER: {
      ((Asset_Shader*) header)->shader_code = (char*) file_data;
    } break;
    default: {
      return INVALID_ARGUMENT;
    } break;
  }

  server->platform_api->file_close_fn(file_handle);
  return OK;
}

bool Asset_ServerGetAsset(Asset_Server* server,
                          Asset_Handle handle,
                          Asset_Header** asset_header) {
  ASSERT(server != NULL);

  for (Asset_Header* header = server->sentinel.next;
       header != &server->sentinel;
       header = header->next) {
    if (header->handle == handle) {
      *asset_header = header;
      return true;
    }
  }
  return false;
}

Status Asset_ServerFreeAsset(Asset_Server* server, Asset_Handle handle) {
  ASSERT(server != NULL);

  Asset_Header* p_header = &server->sentinel;
  Asset_Header* c_header = server->sentinel.next;
  while (c_header != &server->sentinel) {
    if (c_header->handle == handle) { break; }
    p_header = p_header->next;
    c_header = c_header->next;
  }
  if (c_header->handle != handle) { return OK; }
  p_header->next = c_header->next;

  switch (c_header->type) {
    case AT_SHADER: {
      Allocator_FreeBlock(
          &server->allocator, ((Asset_Shader*) c_header)->shader_code);
    } break;
    default: {
      return INVALID_ARGUMENT;
    } break;
  }

  Allocator_FreeBlock(&server->allocator, c_header);

  return OK;
}
