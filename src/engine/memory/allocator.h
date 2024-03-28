#ifndef ENGINE_MEMORY_ALLOCATOR_H
#define ENGINE_MEMORY_ALLOCATOR_H

#include <common/bool.h>
#include <common/status.h>
#include <engine/memory/memory.h>
#include <inttypes.h>

typedef struct MemBlockHeader {
  size_t block_size;
  bool used;
  struct MemBlockHeader* prev;
  struct MemBlockHeader* next;
} MemBlockHeader;

typedef struct Allocator {
  Arena arena;
  MemBlockHeader sentinel;
} Allocator;

Status Allocator_Init(Allocator* allocator, Arena arena);
void* Allocator_AllocateBlock(Allocator* allocator, size_t block_size);
void Allocator_FreeBlock(Allocator* allocator, void* block);

#endif
