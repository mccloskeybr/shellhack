#include <platform/platform_graphics.h>

#include <common/status.h>
#include <platform_runner.h>
#include <windows.h>
#include <malloc.h>


struct Platform_Window {
	HWND handle;
};

struct Win32_WindowDimensions {
	int width;
	int height;
};

const static int BYTES_PER_PIXEL = 4;

static Win32_WindowDimensions
Win32_GetWindowDimensions(const HWND window) {
	RECT client_rect;
	GetClientRect(window, &client_rect);

	Win32_WindowDimensions dimensions;
	dimensions.width = client_rect.right - client_rect.left;
	dimensions.height = client_rect.bottom - client_rect.top;
	return dimensions;
}

static void
Win32_FormatEnginePixelBuffer(
		const PixelBuffer* const pixel_buffer,
		void* formatted_pixel_data) {
	const int32_t a_pitch = pixel_buffer->width * sizeof(Pixel);
	uint8_t* a_row = (uint8_t*) pixel_buffer->buffer;

	const int32_t b_pitch = pixel_buffer->width * BYTES_PER_PIXEL;
	uint8_t* b_row = (uint8_t*) formatted_pixel_data;

	for (int y = 0; y < pixel_buffer->height; ++y) {
		Pixel* a_pixel = (Pixel*) a_row;
		uint32_t* b_pixel = (uint32_t*) b_row;

		for (int x = 0; x < pixel_buffer->width; ++x) {
			*b_pixel = a_pixel->r << 16 | a_pixel->g << 8 | a_pixel->b;

			a_pixel++;
			b_pixel++;
		}

		a_row += a_pitch;
		b_row += b_pitch;
	}
}

static Status
Win32_DisplayBufferInWindow(
		const HWND window,
		const PixelBuffer* const pixel_buffer) {
	// TODO: this can probably be global?
	void* formatted_pixel_data = malloc(
			(pixel_buffer->width * pixel_buffer->height) * BYTES_PER_PIXEL);
	if (formatted_pixel_data == NULL) { return RESOURCE_EXHAUSTED; }

	Win32_FormatEnginePixelBuffer(pixel_buffer, formatted_pixel_data);

	BITMAPINFO info = {};
	info.bmiHeader.biSize = sizeof(info.bmiHeader);
	info.bmiHeader.biWidth = pixel_buffer->width;
	info.bmiHeader.biHeight = -pixel_buffer->height;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = BYTES_PER_PIXEL * 8;
	info.bmiHeader.biCompression = BI_RGB;

	Win32_WindowDimensions dimensions = Win32_GetWindowDimensions(window);

	StretchDIBits(
		GetDC(window),
		0, 0, dimensions.width, dimensions.height,
		0, 0, pixel_buffer->width, pixel_buffer->height,
		formatted_pixel_data,
		&info,
		DIB_RGB_COLORS, SRCCOPY);

	free(formatted_pixel_data);
	return OK;
}

LRESULT CALLBACK
Win32_MainWindowCallback(HWND window,
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

static Status
Win32_CreateWindow(HWND* window) {
	HINSTANCE instance = GetModuleHandle(NULL);

	WNDCLASSA window_class = {};
	window_class.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	window_class.lpfnWndProc = Win32_MainWindowCallback;
	window_class.hInstance = instance;
	window_class.lpszClassName = "GameWindowClass";
	if (!RegisterClassA(&window_class)) { return INTERNAL; }

	*window = CreateWindowExA(0,
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
	if (*window == 0) { return INTERNAL; }
	return OK;
}

Status
Platform_CreateWindow(
	Platform_Window** platform_window) {

	HWND window;
	RETURN_IF_ERROR(Win32_CreateWindow(&window));

	*platform_window = (Platform_Window*) malloc(sizeof(Platform_Window));
	if (*platform_window == NULL) { return RESOURCE_EXHAUSTED; }
	(*platform_window)->handle = window;
	return OK;
}

Status
Platform_RenderBuffer(
	const Platform_Window* const window,
	const PixelBuffer* const buffer) {

	return Win32_DisplayBufferInWindow(window->handle, buffer);
}
