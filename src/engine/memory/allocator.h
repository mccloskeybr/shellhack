#ifndef ENGINE_MEMORY_ALLOCATOR_H
#define ENGINE_MEMORY_ALLOCATOR_H

#include <common/status.h>
#include <engine/memory/memory.h>
#include <inttypes.h>

struct MemBlockHeader {
  size_t block_size;
  bool used;
  MemBlockHeader* prev;
  MemBlockHeader* next;
};

struct Allocator {
  Arena arena;
  MemBlockHeader sentinel;
};

Status Allocator_Init(
    Allocator* allocator,
    Arena arena);

void* Allocator_AllocateBlock(
    Allocator* allocator,
    size_t block_size);

void Allocator_FreeBlock(
    Allocator* allocator,
    void* block);

#endif
