#include <platform/platform.h>

#include <common/log.h>
#include <common/status.h>
#include <engine/render/render.h>
#include <platform/platform_runner.h>
#include <windows.h>
#include <malloc.h>
#include <gl/gl.h>

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_FLAGS_ARB                     0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB                 0x0001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define GL_FRAMEBUFFER_SRGB                       0x8DB9
typedef HGLRC WINAPI WglCreateContextAttribsArbFunc(
    HDC hDC, HGLRC hShareContext, const int *attribList);
typedef struct Win32_OpenGLAPI {
  WglCreateContextAttribsArbFunc* wgl_create_context_attribs_arb_func;
} Win32_OpenGLAPI;

typedef struct Platform_Graphics {
  HWND window;
} Platform_Graphics;

static Status Win32_CreateOpenGLAPI(Win32_OpenGLAPI* api, HDC window_dc) {
  HGLRC open_gl_rc = wglCreateContext(window_dc);
  if (open_gl_rc == NULL) { return INTERNAL; }
  if (!wglMakeCurrent(window_dc, open_gl_rc)) { return INTERNAL; }

  *api = (Win32_OpenGLAPI){};
  api->wgl_create_context_attribs_arb_func =
    (WglCreateContextAttribsArbFunc*) wglGetProcAddress("wglCreateContextAttribsARB");

  wglDeleteContext(open_gl_rc);
  return OK;
}

static void Win32_SetPixelFormat(HDC window_dc) {
  PIXELFORMATDESCRIPTOR desired_pixel_format = {};
  desired_pixel_format.nSize = sizeof(desired_pixel_format);
  desired_pixel_format.nVersion = 1;
  desired_pixel_format.iPixelType = PFD_TYPE_RGBA;
  desired_pixel_format.dwFlags =
    PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
  desired_pixel_format.cColorBits = 32;
  desired_pixel_format.cAlphaBits = 8;
  desired_pixel_format.iLayerType = PFD_MAIN_PLANE;

  int32_t suggested_pixel_format_idx = ChoosePixelFormat(
      window_dc, &desired_pixel_format);
  PIXELFORMATDESCRIPTOR suggested_pixel_format_desc;
  DescribePixelFormat(window_dc, suggested_pixel_format_idx,
      sizeof(suggested_pixel_format_desc), &suggested_pixel_format_desc);
  SetPixelFormat(window_dc, suggested_pixel_format_idx,
      &suggested_pixel_format_desc);
}

static Status Win32_InitOpenGL(HGLRC* open_gl_rc, HWND window) {
  HDC window_dc = GetDC(window);
  Win32_SetPixelFormat(window_dc);

  Win32_OpenGLAPI open_gl_api;
  RETURN_IF_ERROR(Win32_CreateOpenGLAPI(&open_gl_api, window_dc));

  // TODO: legacy fallback?
  if (open_gl_api.wgl_create_context_attribs_arb_func == NULL) {
    LOG_ERROR("Missing OpenGL wglCreateContextAttribsArb!");
    return INTERNAL;
  }

  const int open_gl_attributes[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 0,
    WGL_CONTEXT_FLAGS_ARB, 0,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    0
  };
  *open_gl_rc = open_gl_api.wgl_create_context_attribs_arb_func(
      window_dc, 0, open_gl_attributes);
  if (*open_gl_rc == NULL) { return INTERNAL; }
  if (!wglMakeCurrent(window_dc, *open_gl_rc)) { return INTERNAL; }

  // TODO: check if available.
  glEnable(GL_FRAMEBUFFER_SRGB);

  ReleaseDC(window, window_dc);

  return OK;
}

LRESULT CALLBACK Win32_MainWindowCallback(
    HWND window,
    UINT message,
    WPARAM w_param,
    LPARAM l_param) {
  switch(message) {
    case WM_CLOSE:
    case WM_DESTROY: {
      Platform_QuitGame();
    } break;

    // TODO: move to separate handler
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP: {
      OutputDebugStringA("key event\n");
    } break;

    default: {
      return DefWindowProcA(window, message, w_param, l_param);
    } break;
  }
  LRESULT result = 0;
  return result;
}

static Status Win32_CreateWindow(HWND* window) {
  HMODULE instance = GetModuleHandle(NULL);

  WNDCLASSA window_class = {};
  window_class.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
  window_class.lpfnWndProc = Win32_MainWindowCallback;
  window_class.hInstance = instance;
  window_class.lpszClassName = "GameWindowClass";
  if (!RegisterClassA(&window_class)) {
    LOG_ERROR("Could not register window!");
    return INTERNAL;
  }

  *window = CreateWindowExA(
      0,
      window_class.lpszClassName,
      "Test",
      WS_OVERLAPPEDWINDOW|WS_VISIBLE,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      0,
      0,
      instance,
      0);
  if (*window == 0) {
    LOG_ERROR("Could not create window!");
    return INTERNAL;
  }
  return OK;
}

Status Platform_InitializeGraphics(Platform_Graphics** platform_graphics) {
  HWND window;
  RETURN_IF_ERROR(Win32_CreateWindow(&window));
  HGLRC open_gl_rc;
  RETURN_IF_ERROR(Win32_InitOpenGL(&open_gl_rc, window));

  *platform_graphics = (Platform_Graphics*) malloc(sizeof(Platform_Graphics));
  if (*platform_graphics == NULL) { return RESOURCE_EXHAUSTED; }
  (*platform_graphics)->window = window;
  return OK;
}

Status Platform_SwapBuffer(const Platform_Graphics* const graphics) {
  HDC window_dc = GetDC(graphics->window);
  SwapBuffers(window_dc);
  ReleaseDC(graphics->window, window_dc);

  return OK;
}
