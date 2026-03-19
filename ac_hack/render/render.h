#pragma once
#include <windows.h>

namespace Render {
    //called every frame from SwapBuffers hook
    void OnSwapBuffers(HDC hDc);
    void Shutdown();
}