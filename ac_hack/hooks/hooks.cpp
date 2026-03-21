#include "hooks.h"
#include "../render/render.h"
#include "../menu/menu.h" 
#include "../globals/globals.h"
#include "../entity.h"
#include "../gameManager.h"
#include "../cheatMemory/cheatMemory.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <thread>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//i dont know why this shit crashes game when the bot tries to shoot (if you try to call orig func in the hooked one)

//typedef void(__cdecl* RayDamageType)(float* from, float* to, int* gunOwner);
//RayDamageType oRayDamage = nullptr; 
//
////raydamage hook
//void __cdecl hkRayDamage(float* from, float* to, int* gunOwner) {
//    if (gunOwner != (int*)(uintptr_t) & (*Globals::gameManager->_localPlayer)) {
//        oRayDamage(from, to, gunOwner);
//        return;
//    }
//    else {
//        if (!Globals::gameManager || !Globals::gameManager->_localPlayer || Globals::gameManager->_playersCounter <= 1) {
//            return;
//        }
//        else {
//
//            if (!Globals::gameManager->_localPlayer
//                || Globals::gameManager->_localPlayer->_health <= 0) {
//                return;
//            }
//
//            Entity* localPlayer = Globals::gameManager->_localPlayer;
//            Entity** entityList = (Entity**)Globals::gameManager->_entities;
//
//            float yaw = localPlayer->_angles.x;
//            float pitch = localPlayer->_angles.y;
//            float radYaw = (yaw - 90.0f) * (M_PI / 180.0f);
//            float radPitch = pitch * (M_PI / 180.0f);
//
//            Vector3 forward;
//            forward.x = cos(radPitch) * cos(radYaw);
//            forward.y = cos(radPitch) * sin(radYaw);
//            forward.z = sin(radPitch);
//
//            static int currentBotIndex = 0;
//            Entity* target = nullptr;
//            int maxPlayers = Globals::gameManager->_playersCounter;
//
//            for (int i = 0; i < maxPlayers; i++) {
//                currentBotIndex++;
//
//
//                if (currentBotIndex >= maxPlayers) {
//                    currentBotIndex = 1;
//                }
//
//                if (!entityList[currentBotIndex]) continue;
//                Entity* ent = entityList[currentBotIndex];
//
//                if (ent->_health > 0) {
//                    target = ent;
//                    break;
//                }
//            }
//
//            //launch the enemy like a projectile
//            if (target) {
//                //put himin front of our face
//                target->_pos.x = localPlayer->_pos.x + (forward.x * 3.0f);
//                target->_pos.y = localPlayer->_pos.y + (forward.y * 3.0f);
//                target->_pos.z = localPlayer->_pos.z + (forward.z * 3.0f);
//
//
//                float launchForce = 10.0f;//speed impulse
//                target->_velocity.x = forward.x * launchForce;
//                target->_velocity.y = forward.y * launchForce;
//                target->_velocity.z = forward.z * launchForce;
//            }
//
//            /*if (oRayDamage) {
//                oRayDamage(from, to, gunOwner);
//            }*/
//        }
//    }
//}


typedef void(__fastcall* AttackPhysicsType)(int16_t* thisptr, void* edx, float* from, float* to);
AttackPhysicsType oAttackPhysics = nullptr;

void __fastcall hkAttackPhysics(int16_t* owner, void* edx, float* from, float* to) {
    if (Globals::Settings::Hooks::noSpreadANDRecoilOn) {
        return;
    }
    else {
        __try {
            oAttackPhysics(owner, edx, from, to);
        }
        __except (Memory::ExceptionFilter(GetExceptionCode(), GetExceptionInformation())) {
            printf("programm was healed after crash\n");
        }
       
    }
}

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
        //uintptr_t shootAddr = (uintptr_t)Globals::Addresses::FunctionAddresses::rayDamage;
        //oRayDamage = (RayDamageType)shootAddr;
        //MH_CreateHook((void*)shootAddr, &hkRayDamage, reinterpret_cast<void**>(&oRayDamage));

        uintptr_t atackPhysicsAddr = (uintptr_t)Globals::Addresses::FunctionAddresses::attackPhysics;
        oAttackPhysics = (AttackPhysicsType)atackPhysicsAddr;
        MH_CreateHook((void*)atackPhysicsAddr, &hkAttackPhysics, reinterpret_cast<void**>(&oAttackPhysics));

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