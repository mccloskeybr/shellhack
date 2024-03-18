#include <engine/memory/list.h>

#include <common/status.h>
#include <common/macros.h>
#include <engine/memory/allocator.h>
#include <inttypes.h>

  // TODO: byte copy is pretty slow.
static void List_CopyBytes(
    void* src,
    void* dest,
    int32_t n_bytes) {
  uint8_t* src_byte = (uint8_t*) src;
  uint8_t* dest_byte = (uint8_t*) dest;
  for (int32_t i = 0; i < n_bytes; i++) {
    *dest_byte++ = *src_byte++;
  }
}

static void List_Resize(List* list) {
  int32_t new_capacity = list->capacity * LIST_GROW_FACTOR;
  void* new_block = Allocator_AllocateBlock(
      list->allocator, new_capacity * list->elem_size);
  ASSERT(new_block != NULL);
  List_CopyBytes(list->arr, new_block, list->size * list->elem_size);
  Allocator_FreeBlock(list->allocator, list->arr);
  list->arr = new_block;
  list->capacity = new_capacity;
}

List List_Create(
    Allocator* allocator,
    size_t elem_size) {
  List list = {};
  list.capacity = LIST_DEFAULT_CAPACITY;
  list.elem_size = elem_size;
  list.allocator = allocator;
  list.arr = Allocator_AllocateBlock(
      list.allocator, list.capacity * list.elem_size);
  ASSERT(list.arr != NULL);
  return list;
}

List List_CreateCopy(
    Allocator* allocator,
    List* to_copy) {
  ASSERT(to_copy->arr != NULL);
  List list = {};
  list.allocator = allocator;
  list.size = to_copy->size;
  list.capacity = to_copy->capacity;
  list.elem_size = to_copy->elem_size;
  list.arr = Allocator_AllocateBlock(
      list.allocator, list.capacity * list.elem_size);
  ASSERT(list.arr != NULL);

  uint8_t* src = (uint8_t*) to_copy->arr;
  uint8_t* dest = (uint8_t*) list.arr;
  List_CopyBytes(src, dest, list.size * list.elem_size);

  return list;
}

void List_Free(
    List* list) {
  ASSERT(list->arr != NULL);
  Allocator_FreeBlock(list->allocator, list->arr);
}

void List_Add(
    List* list,
    void* to_add) {
  ASSERT(list->arr != NULL);
  if (list->size == list->capacity) { List_Resize(list); }
  ASSERT(list->size < list->capacity);

  uint8_t* src = (uint8_t*) to_add;
  uint8_t* dest = (uint8_t*) list->arr + list->size * list->elem_size;
  List_CopyBytes(src, dest, list->elem_size);
  list->size++;
}

void* List_Get(
    List* list,
    uint32_t index) {
  ASSERT(list->arr != NULL);
  ASSERT(index < list->size);
  return (uint8_t*) list->arr + index * list->elem_size;
}

void List_Remove(
    List* list,
    uint32_t index) {
  ASSERT(list->arr != NULL);
  ASSERT(index < list->size);

  uint8_t* src = (uint8_t*) list->arr + (list->size - 1) * list->elem_size;
  uint8_t* dest = (uint8_t*) list->arr + index * list->elem_size;
  List_CopyBytes(src, dest, list->elem_size);
  list->size--;
}
