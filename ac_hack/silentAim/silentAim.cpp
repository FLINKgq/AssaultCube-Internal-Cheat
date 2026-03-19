#include "pch.h"
#include "silentAim.h"
#include "../entity.h"
#include "../gameManager.h"
#include "../globals/globals.h"
#include "../cheatMemory/cheatMemory.h"


namespace {
    std::thread _mainThread;

    typedef void(__cdecl* RayDamageType)(float* from, float* to, int* gunOwner);

    void shoot(float* from, float* to, Entity* gunOwner) {
        uintptr_t address = Globals::Addresses::baseAddress + Globals::Addresses::FunctionOffsets::rayDamage;
        RayDamageType rayDamage = (RayDamageType)address;

      /*  std::cout << "from = " << from << std::endl;
        std::cout << "to = " << to << std::endl;
        std::cout << "lp = " << gunOwner << std::endl;*/

        __try {
            rayDamage(from, to, (int*)gunOwner);
        }
        __except (Memory::ExceptionFilter(GetExceptionCode(), GetExceptionInformation())) {
            printf("programm was healed after crash\n");
        }
    }

    void mainLoop() {
        while (Globals::cheatIsEnabled) {
            if (Globals::Settings::SilentAim::isEnabled && Globals::gameManager) {
                if (Globals::gameManager->_playersCounter >= 1) {
                    Entity* localPlayer = Globals::gameManager->_localPlayer;
                    if (localPlayer) {
                        Entity** entityList = (Entity**)Globals::gameManager->_entities;

                        for (int i = 1; i < Globals::gameManager->_playersCounter; i++) {
                            Entity* currentEnt = entityList[i];
                            if (currentEnt) {
                                if (currentEnt->_currentTeam == localPlayer->_currentTeam && !Globals::Settings::Aimbot::friendlyFireOn) continue;
                                shoot(
                                    (float*)&localPlayer->_headPos,
                                    (float*)&currentEnt->_headPos,
                                    localPlayer);
                            }
                        }
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}


namespace SilentAim {
    void start() {
        _mainThread = std::thread(mainLoop);
    }

    void shutdown() {
        std::cout << "shutdown SilentAim!\n";
        Globals::Settings::SilentAim::isEnabled = false;
        if (_mainThread.joinable()) {
            _mainThread.join();
        }
    }
}



