#ifndef ENGINE_RENDER_RENDER_H
#define ENGINE_RENDER_RENDER_H

#include <engine/render/camera.h>
#include <engine/render/color.h>
#include <engine/world/entity/entity.h>
#include <inttypes.h>

typedef struct Pixel {
  Color color;
} Pixel;

typedef struct PixelBuffer {
  Pixel* buffer;
  int32_t width;
  int32_t height;
} PixelBuffer;

void Render_FlushScreen(
    PixelBuffer* pixel_buffer,
    Color color);

void Render_DrawEntitiesAroundCamera(
    PixelBuffer* pixel_buffer,
    const Camera* const camera,
    struct World* world);

#endif
