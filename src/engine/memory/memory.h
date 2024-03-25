#ifndef ENGINE_MEMORY_H
#define ENGINE_MEMORY_H

#include <common/status.h>
#include <inttypes.h>

// must be initialized to 0.
typedef struct Memory {
  void* permanent_storage;
  uint64_t permanent_storage_capacity;
  uint64_t permanent_storage_size;

  void* transient_storage;
  uint64_t transient_storage_capacity;
  uint64_t transient_storage_size;
} Memory;

typedef struct Arena {
  void* memory;
  size_t capacity;
  size_t size;
} Arena;

Status Memory_InitializePermArena(
    Arena* arena,
    Memory* memory,
    size_t capacity);

Status Memory_InitializeTransArena(
    Arena* arena,
    Memory* memory,
    size_t capacity);

void* Arena_PushSize(
    Arena* arena,
    size_t size);

#endif
