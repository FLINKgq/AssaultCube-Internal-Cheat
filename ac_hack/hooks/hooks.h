#pragma once
#include <windows.h>
#include "MinHook.h"

namespace Hooks {
    typedef BOOL(__stdcall* tSwapBuffers)(HDC hDc);
    typedef LRESULT(__stdcall* tWndProc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    typedef int(__cdecl* tSDL_SetRelativeMouseMode)(int enabled);

    extern tSwapBuffers oSwapBuffers;
    extern tWndProc oWndProc;
    extern tSDL_SetRelativeMouseMode oSDL_SetRelativeMouseMode;

    extern HWND window;

    BOOL __stdcall hkSwapBuffers(HDC hDc);
    LRESULT __stdcall hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    int __cdecl hkSDL_SetRelativeMouseMode(int enabled);

    void Init();
    void Shutdown();
}