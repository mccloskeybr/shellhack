#ifndef COMMON_V3
#define COMMON_V3

typedef union V3 {
  struct {
    float x;
    float y;
    float z;
  };
  struct {
    float r;
    float g;
    float b;
  };
  float e[3];
} V3;

#endif
