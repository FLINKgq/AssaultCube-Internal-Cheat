#include "pch.h"
#include "littleFeatures.h"
#include "../entity.h"
#include "../gameManager.h"
#include "../globals/globals.h"
#include "../currentWeapon.h"

namespace {
    std::thread _mainThread;
    void mainLoop() {
        while (Globals::cheatIsEnabled) {
            if (Globals::Settings::LittleFeatures::infiniteAmmoOn 
                ||Globals::Settings::LittleFeatures::rapidFireOn
                ||Globals::Settings::LittleFeatures::godModeOn) {
                if (Globals::gameManager) {
                    Entity* localPlayer = Globals::gameManager->_localPlayer;
                    if (localPlayer) {
                        CurrentWeapon* currentWeapon = localPlayer->_currentWeaponPtr;
                        if (currentWeapon) {
                            //inf ammo
                            if (Globals::Settings::LittleFeatures::infiniteAmmoOn) {
                                *(currentWeapon->_ammoPtr) = 100;
                            }
                            //rapid fire 
                            if (Globals::Settings::LittleFeatures::rapidFireOn) {
                                *(currentWeapon->_coolDown) = 0;
                            }
                            //god mode
                            if (Globals::Settings::LittleFeatures::godModeOn) {
                                localPlayer->_health = 777;
                                localPlayer->_armor = 777;
                            }
                        }
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

namespace LittleFeatures {
    void shutdown() {
        std::cout << "shutdown LittleFeatures!\n";
        Globals::Settings::LittleFeatures::infiniteAmmoOn = false;
        Globals::Settings::LittleFeatures::rapidFireOn = false;
        Globals::Settings::LittleFeatures::godModeOn = false;
        if (_mainThread.joinable()) {
            _mainThread.join();
        }
    }

    void start() {
        _mainThread = std::thread(mainLoop);
    }
}

