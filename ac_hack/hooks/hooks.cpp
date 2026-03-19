#include "hooks.h"
#include "../render/render.h"
#include "../menu/menu.h" 
#include "../globals/globals.h"
#include <thread>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Hooks {
    tSwapBuffers oSwapBuffers = nullptr;
    tWndProc oWndProc = nullptr;
    tSDL_SetRelativeMouseMode oSDL_SetRelativeMouseMode = nullptr;
    HWND window = nullptr;

    void Init() {
        MH_Initialize();

        void* swapBuffersAddr = GetProcAddress(GetModuleHandleA("gdi32.dll"), "SwapBuffers");
        if (!swapBuffersAddr) swapBuffersAddr = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
        MH_CreateHook(swapBuffersAddr, &hkSwapBuffers, reinterpret_cast<void**>(&oSwapBuffers));

        HMODULE hSDL = GetModuleHandleA("SDL2.dll");
        if (hSDL) {
            void* sdlSetRelMouseAddr = GetProcAddress(hSDL, "SDL_SetRelativeMouseMode");
            MH_CreateHook(sdlSetRelMouseAddr, &hkSDL_SetRelativeMouseMode, reinterpret_cast<void**>(&oSDL_SetRelativeMouseMode));
        }

        MH_EnableHook(MH_ALL_HOOKS);
    }

    void Shutdown() {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(MH_ALL_HOOKS);
        MH_Uninitialize();

        if (oWndProc && window) {
            SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
        }
    }


    BOOL __stdcall hkSwapBuffers(HDC hDc) {
        Render::OnSwapBuffers(hDc);
        return oSwapBuffers(hDc);
    }


    LRESULT __stdcall hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_CLOSE || uMsg == WM_DESTROY) {
            Globals::cheatIsEnabled = false;   
        }

        if (uMsg == WM_KEYDOWN && wParam == VK_F3 && ((lParam & (1 << 30)) == 0)) {
            Menu::Toggle();

            if (oSDL_SetRelativeMouseMode) {
                oSDL_SetRelativeMouseMode(Menu::IsOpen() ? 0 : 1);
            }
            return true;
        }

        if (Menu::IsOpen()) {
            ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

            if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) return true;
            if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) return true;
            if (uMsg == WM_CHAR) return true;
        }
        return CallWindowProcA(oWndProc, hWnd, uMsg, wParam, lParam);
    }

    int __cdecl hkSDL_SetRelativeMouseMode(int enabled) {
        if (Menu::IsOpen()) return oSDL_SetRelativeMouseMode(0);
        return oSDL_SetRelativeMouseMode(enabled);
    }
}