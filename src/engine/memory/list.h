#ifndef COMMON_LIST_H
#define COMMON_LIST_H

#include <common/status.h>
#include <engine/memory/allocator.h>
#include <inttypes.h>

#define LIST_DEFAULT_CAPACITY 2
#define LIST_GROW_FACTOR 2

#define LIST_CREATE(allocator, type) \
  List_Create(allocator, sizeof(type))

#define LIST_ADD(list, e, type) \
  {                             \
    type _elem = e;             \
    List_Add(list, &_elem);     \
  }

#define LIST_GET(list, i, type) \
  *(type*)List_Get(list, i)

#define LIST_REMOVE(list, i) \
  List_Remove(list, i)


struct List {
  void* first;
  uint32_t size;
  uint32_t capacity;
  size_t elem_size;
  Allocator* allocator;
};

List List_Create(
    Allocator* allocator,
    size_t elem_size);

void List_Add(
    List* list,
    void* elem);

// TODO: maybe a macro can help templatize this
void* List_Get(
    List* list,
    uint32_t index);

void List_Remove(
    List* list,
    uint32_t index);

#endif
