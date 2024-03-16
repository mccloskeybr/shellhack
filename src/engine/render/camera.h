#ifndef RENDER_CAMERA_H
#define RENDER_CAMERA_H

#include <common/status.h>
#include <engine/shapes/rectangle.h>

// TODO: scaling? etcetera?
struct Camera {
  Rectangle viewport;
};

inline V2 WorldPosToCameraRelative(const Camera* const camera, V2 pos) {
  UNREACHABLE;
  return pos;
}

#endif
