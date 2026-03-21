#include "pch.h"
#include "core.h"
#include "MinHook.h"
#include "utils.h"
#include "gameManager.h"
#include "aimbot/aimbot.h"
#include "hooks/hooks.h"
#include "render/render.h"
#include "globals/globals.h"
#include "LittleFeatures/LittleFeatures.h"
#include "cheatMemory/cheatMemory.h"
#include "movement/movement.h"
#include "silentAim/silentAim.h"

#pragma comment(lib, "opengl32.lib")

#if defined(_WIN64)
#ifdef _DEBUG
#pragma comment(lib, "libMinHook.x64-d.lib")
#else 
#pragma comment(lib, "libMinHook.x64.lib")
#endif
#else
#ifdef _DEBUG 
#pragma comment(lib, "libMinHook.x86-d.lib")
#else 
#pragma comment(lib, "libMinHook.x86.lib")
#endif
#endif

#ifndef _DEBUG
#error "Compile ONLY in Debug mode!!!! Release mode shitass optimizations break Silent Aim."
#endif

namespace Core {
	void init(void* hModule) {
		Globals::init();
        std::thread cheatThread(mainLoop, hModule);
        cheatThread.detach();
	}

	void mainLoop(void* hM) {

#ifdef _DEBUG //we can do it this way because we don't have any heavy calculations inside std::cout anywhere in the code
            ShowConsole();
#endif
            HMODULE hModule = static_cast<HMODULE>(hM);
            {
                Globals::init();
                if (!Globals::gameManager || !Globals::Addresses::baseAddress) {
                    FreeLibraryAndExitThread(hModule, 0);
                }

                Hooks::Init();
                Aimbot::start();
                LittleFeatures::start();
                Movement::start();
                SilentAim::start();

                while ( (!(GetAsyncKeyState(VK_END) & 1)) && Globals::cheatIsEnabled) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }

                Globals::cheatIsEnabled = false;
                Aimbot::shutdown();
                LittleFeatures::shutdown();
                Movement::shutdown();
                SilentAim::shutdown();
                Hooks::Shutdown();
                Render::Shutdown();
            }
            HideConsole();
            FreeLibraryAndExitThread(hModule, 0);
	}
}