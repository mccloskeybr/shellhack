#ifndef PLATFORM_GRAPHICS
#define PLATFORM_GRAPHICS

#include <common/status.h>
#include <engine/render/render.h>

struct Platform_Window;

Status
Platform_CreateWindow(Platform_Window** window);

Status
Platform_RenderBuffer(
    const Platform_Window* window,
    const PixelBuffer* const buffer);

#endif
