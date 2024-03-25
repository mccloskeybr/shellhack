#ifndef COMMON_V2
#define COMMON_V2

// TODO: intrinsics
#include <math.h>

typedef union V2 {
  struct {
    float x;
    float y;
  };
  struct {
    float u;
    float v;
  };
  float e[2];
} V2;

inline V2 V2_ScalarMult(float c, V2 v) {
  V2 result;
  result.x = c * v.x;
  result.y = c * v.y;
  return result;
}

inline V2 V2_Add(V2 a, V2 b) {
  V2 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return result;
}

inline V2 V2_Sub(V2 a, V2 b) {
  V2 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  return result;
}

inline float V2_Inner(V2 a, V2 b) {
  return a.x * b.x + a.y * b.y;
}

inline float V2_Dot(V2 a, V2 b) {
  return V2_Inner(a, b);
}

inline float V2_LengthSq(V2 a) {
  return V2_Dot(a, a);
}

inline float V2_Length(V2 a) {
  return sqrtf(V2_LengthSq(a));
}

inline V2 V2_Normalize(V2 a) {
  return V2_ScalarMult(1.0f / V2_Length(a), a);
}

#endif
