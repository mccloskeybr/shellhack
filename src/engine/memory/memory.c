#include <engine/memory/memory.h>

#include <common/macros.h>
#include <common/status.h>
#include <inttypes.h>

static Arena
CreateArena(void* memory, size_t capacity) {
  Arena arena;
  arena.memory = memory;
  arena.capacity = capacity;
  arena.size = 0;
  return arena;
}

Status Memory_InitializePermArena(
    Arena* arena,
    Memory* memory,
    size_t capacity) {
  if (memory->permanent_storage_size + capacity >
      memory->permanent_storage_capacity) {
    return RESOURCE_EXHAUSTED;
  }

  *arena = CreateArena(
      (uint8_t*)memory->permanent_storage + memory->permanent_storage_size,
      capacity);
  memory->permanent_storage_size += capacity;
  return OK;
}

Status Memory_InitializeTransArena(
    Arena* arena,
    Memory* memory,
    size_t capacity) {
  if (memory->transient_storage_size + capacity >
      memory->transient_storage_capacity) {
    return RESOURCE_EXHAUSTED;
  }

  *arena = CreateArena(
      (uint8_t*)memory->transient_storage + memory->transient_storage_size,
      capacity);
  memory->transient_storage_size += capacity;
  return OK;
}

void* Arena_PushSize(
    Arena* arena,
    size_t size) {
  if (arena->size + size > arena->capacity) {
    return NULL;
  }

  void* block = (uint8_t*)arena->memory + arena->size;
  arena->size += size;
  return block;
}
