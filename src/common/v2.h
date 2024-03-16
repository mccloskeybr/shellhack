#ifndef COMMON_V2
#define COMMON_V2

// TODO: intrinsics
#include <math.h>

union V2 {
  struct {
    float x;
    float y;
  };
  struct {
    float u;
    float v;
  };
  float e[2];
};

inline V2 operator*(float c, V2 v) { return {c*v.x, c*v.y}; }
inline V2 operator*(V2 v, float c) { return c*v; }
inline V2& operator*=(V2& v, float c) { v = c*v; return v; }

inline V2 operator+(V2 a, V2 b) { return {a.x + b.x, a.y + b.y}; }
inline V2& operator+=(V2& a, V2 b) { a = a + b; return a; }

inline V2 operator-(V2 a, V2 b) { return {a.x - b.x, a.y - b.y}; }
inline V2& operator-=(V2& a, V2 b) { a = a - b; return a; }

inline float Inner(V2 a, V2 b) { return a.x * b.x + a.y * b.y; }
inline float Dot(V2 a, V2 b) { return Inner(a, b); }

inline float LengthSq(V2 a) { return Dot(a, a); }
inline float Length(V2 a) { return sqrtf(LengthSq(a)); }

inline V2 Normalize(V2 a) { return a * (1.0f / Length(a)); }

#endif
