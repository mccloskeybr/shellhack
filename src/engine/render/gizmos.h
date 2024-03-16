#ifndef ENGINE_RENDER_GIZMOS_H
#define ENGINE_RENDER_GIZMOS_H

#include <common/v2.h>
#include <engine/render/render.h>
#include <engine/render/color.h>
#include <inttypes.h>

void Gizmos_DrawRectangle(
    PixelBuffer* pixel_buffer,
    V2 center_point,
    int32_t width,
    int32_t height,
    Color color);

#endif
