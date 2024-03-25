#ifndef ENGINE_SHAPES_RECTANGLE_H
#define ENGINE_SHAPES_RECTANGLE_H

#include <common/v2.h>
#include <common/status.h>

typedef struct Shape_Rect {
  V2 center_point;
  float width;
  float height;
} Shape_Rect;

inline Shape_Rect Shape_RectCenterDims(
    V2 center_point,
    float width,
    float height) {
  Shape_Rect rect;
  rect.center_point = center_point;
  rect.width = width;
  rect.height = height;
  return rect;
}

inline Shape_Rect Shape_RectMinMax(
    V2 min_point,
    V2 max_point) {
  ASSERT(min_point.x < max_point.x);
  ASSERT(min_point.y < max_point.y);

  Shape_Rect rect;
  rect.width = max_point.x - min_point.x;
  rect.height = max_point.y - min_point.y;
  rect.center_point.x = min_point.x + (rect.width / 2.0f);
  rect.center_point.y = min_point.y + (rect.height / 2.0f);
  return rect;
}

#endif
