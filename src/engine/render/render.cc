#include <engine/render/render.h>

#include <common/macros.h>
#include <engine/render/camera.h>
#include <engine/render/gizmos.h>
#include <engine/world/entity/component.h>
#include <engine/world/entity/component_hash.h>
#include <engine/render/color.h>
#include <inttypes.h>

void Render_FlushScreen(
    PixelBuffer* pixel_buffer,
    Color color) {

  for (int32_t i = 0; i < pixel_buffer->width * pixel_buffer->height; i++) {
    pixel_buffer->buffer[i].color = color;
  }
}

void Render_DrawEntitiesAroundCamera(
    PixelBuffer* pixel_buffer,
    const Camera* const camera,
    World* world) {

  ComponentHashMapIter iter;
  for (ComponentHashIter_First(
        &iter,
        &world->component_maps[COMPONENT_TYPE_SPATIAL]);
      !ComponentHashIter_Done(&iter);
      ComponentHashIter_Next(&iter)) {
    V2 screen_pos =
      iter.ptr->component.spatial.world_pos -
      camera->viewport.center_point +
      V2{camera->viewport.width/2.0f, camera->viewport.height/2.0f};
    Gizmos_DrawRectangle(
        pixel_buffer,
        screen_pos,
        50, 50,
        {0.0f, 255.0f, 0.0f});
  }
}
