#include <engine/render/gizmos.h>

#include <common/v2.h>
#include <engine/render/render.h>
#include <engine/render/color.h>
#include <inttypes.h>

static int32_t
Max(int32_t a, int32_t b) {
  return (a > b) ? a : b;
}

static int32_t
Min(int32_t a, int32_t b) {
  return (a < b) ? a : b;
}

void
Gizmos_DrawRectangle(
    PixelBuffer* pixel_buffer,
    V2 center_point,
    int32_t width,
    int32_t height,
    Color color) {

  const int32_t min_x = Max((int32_t)(center_point.x) - width / 2, 0);
  const int32_t max_x = Min((int32_t)(center_point.x) + width / 2, pixel_buffer->width);
  const int32_t min_y = Max((int32_t)(center_point.y) - height / 2, 0);
  const int32_t max_y = Min((int32_t)(center_point.y) + height / 2, pixel_buffer->height);

  Pixel* row = pixel_buffer->buffer + pixel_buffer->width * min_y + min_x;
  for (int32_t y = min_y; y < max_y; y++) {
    Pixel* pixel = row;
    for (int32_t x = min_x; x < max_x; x++) {
      pixel->color = color;
      pixel++;
    }
    row += pixel_buffer->width;
  }
}
