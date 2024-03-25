#include <common/mem_util.h>

void Memory_ZeroRegion(void* region, size_t size) {
  for (size_t i = 0; i < size; i++) {
    *((uint8_t*)region + i) = 0;
  }
}
