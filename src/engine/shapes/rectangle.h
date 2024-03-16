#ifndef ENGINE_SHAPES_RECTANGLE_H
#define ENGINE_SHAPES_RECTANGLE_H

#include <common/v2.h>
#include <common/status.h>

struct Rectangle {
  V2 center_point;
  float width;
  float height;
};

inline Rectangle RectangleCenterDims(
    V2 center_point,
    float width,
    float height) {
  Rectangle rect;
  rect.center_point = center_point;
  rect.width = width;
  rect.height = height;
  return rect;
}

inline Rectangle RectangleMinMax(
    V2 min_point,
    V2 max_point) {
  ASSERT(min_point.x < max_point.x);
  ASSERT(min_point.y < max_point.y);

  Rectangle rect;
  rect.width = max_point.x - min_point.x;
  rect.height = max_point.y - min_point.y;
  rect.center_point = {
    min_point.x + (rect.width / 2.0f),
    min_point.y + (rect.height / 2.0f)
  };
  return rect;
}

#endif
