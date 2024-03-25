#ifndef COMMON_LIST_H
#define COMMON_LIST_H

#include <common/status.h>
#include <engine/memory/allocator.h>
#include <inttypes.h>

#define LIST_DEFAULT_CAPACITY 2
#define LIST_GROW_FACTOR 2

#define LIST_CREATE(allocator, type)   \
  List_Create(allocator, sizeof(type))

#define LIST_FREE(list)                \
  List_Free(list)

#define LIST_COPY(allocator, list)     \
  List_CreateCopy(allocator, list)

#define LIST_ADD(list, e, type)        \
  {                                    \
    type _elem = e;                    \
    List_Add(list, &_elem);            \
  }

#define LIST_GET(list, i, type)        \
  *(type*)List_Get(list, i)

#define LIST_REMOVE(list, i)           \
  List_Remove(list, i)


typedef struct List {
  void* arr;
  uint32_t size;
  uint32_t capacity;
  size_t elem_size;
  Allocator* allocator;
} List;

List List_Create(
    Allocator* allocator,
    size_t elem_size);

List List_CreateCopy(
    Allocator* allocator,
    List* to_copy);

void List_Free(
    List* list);

void List_Add(
    List* list,
    void* elem);

void* List_Get(
    List* list,
    uint32_t index);

void List_Remove(
    List* list,
    uint32_t index);

#endif
