#include <platform/platform_runner.h>

#include <windows.h>

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode) {

	Platform_Main();
}
