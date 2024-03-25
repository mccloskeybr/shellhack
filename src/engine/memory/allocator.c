#include <engine/memory/allocator.h>

#include <common/status.h>
#include <common/macros.h>
#include <engine/memory/memory.h>

// TODO: this can be faster if blocks are int32 aligned.
static void Allocator_ZeroRegion(
    void* block,
    size_t size) {
  for (size_t i = 0; i < size; i++) {
    *((uint8_t*)block + i) = 0;
  }
}

Status Allocator_Init(
    Allocator* allocator,
    Arena arena) {
  // *allocator = {};
  allocator->arena = arena;
  allocator->sentinel.next = &allocator->sentinel;
  allocator->sentinel.prev = &allocator->sentinel;

  MemBlockHeader* block = (MemBlockHeader*) Arena_PushSize(
      &allocator->arena, allocator->arena.capacity);
  if (block == NULL) { return RESOURCE_EXHAUSTED; }
  // *block = {};
  block->block_size = allocator->arena.capacity - sizeof(MemBlockHeader);
  block->used = false;

  block->prev = &allocator->sentinel;
  block->next = &allocator->sentinel;
  block->prev->next = block;
  block->next->prev = block;

  return OK;
}

void* Allocator_AllocateBlock(
    Allocator* allocator,
    size_t block_size) {
  ASSERT(allocator != NULL);

  // use an adjusted size as it ensures there's always enough room in the event
  // of a split.
  // TODO: consider using formal memory chunks of uniform size as an alternative.
  size_t block_size_adj = block_size + sizeof(MemBlockHeader);

  MemBlockHeader* best_fit = NULL;
  for (MemBlockHeader* ptr = allocator->sentinel.next;
      ptr != &allocator->sentinel;
      ptr = ptr->next) {
    if (ptr->used || block_size_adj > ptr->block_size) { continue; }
    if (best_fit == NULL || best_fit->block_size > ptr->block_size) {
      best_fit = ptr;
    }
  }
  ASSERT(best_fit != NULL);
  ASSERT(best_fit != &allocator->sentinel);

  if (best_fit->block_size > block_size_adj) {
    MemBlockHeader* split =
      (MemBlockHeader*)((uint8_t*)best_fit + sizeof(MemBlockHeader) + block_size_adj);
    split->used = false;
    split->block_size = best_fit->block_size - sizeof(MemBlockHeader) - block_size_adj;
    split->next = best_fit->next;
    split->prev = best_fit;
    ASSERT(split->block_size > 0);

    best_fit->next = split;
  }

  best_fit->used = true;
  best_fit->block_size = block_size_adj;
  return (uint8_t*) best_fit + sizeof(MemBlockHeader);
}

void Allocator_FreeBlock(
    Allocator* allocator,
    void* block) {
  ASSERT(allocator != NULL);
  ASSERT(block != NULL);

  MemBlockHeader* header = (MemBlockHeader*)((uint8_t*)block - sizeof(MemBlockHeader));
  Allocator_ZeroRegion(block, header->block_size);
  header->used = false;

  if (header->prev != &allocator->sentinel && !header->prev->used) {
    MemBlockHeader* to_clear = header;
    header->prev->block_size += sizeof(MemBlockHeader) + header->block_size;
    header->prev->next = header->next;
    header->next->prev = header->prev;
    header = header->prev;
    // *to_clear = {};
  }
  if (header->next != &allocator->sentinel && !header->next->used) {
    MemBlockHeader* to_clear = header->next;
    header->block_size += sizeof(MemBlockHeader) + header->next->block_size;
    header->next = header->next->next;
    header->next->prev = header;
    // *to_clear = {};
  }
}

