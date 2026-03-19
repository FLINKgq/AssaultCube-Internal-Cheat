#include "pch.h"
#include "movement.h"
#include "../entity.h"
#include "../gameManager.h"
#include "../globals/globals.h"

namespace {
    std::thread _speedHackThread;
    std::thread _flyHackThread;

    void speedHackLoop() {
        while (Globals::cheatIsEnabled) {
            if (Globals::gameManager && Globals::Settings::SpeedHack::isEnabled) {
                Entity* localPlayer = Globals::gameManager->_localPlayer;
                if (localPlayer) {

                    //speed hack
                    if (localPlayer->_playerState == 0 && localPlayer->_isOnGround) {
                        if (GetAsyncKeyState('W') & 0x8000
                            || GetAsyncKeyState('A') & 0x8000
                            || GetAsyncKeyState('S') & 0x8000
                            || GetAsyncKeyState('D') & 0x8000) {
                            if (Globals::Settings::SpeedHack::maxSpeed > std::abs(localPlayer->_velocity.x) && Globals::Settings::SpeedHack::maxSpeed > std::abs(localPlayer->_velocity.y)) {
                                localPlayer->_velocity.x *= 1.1f;
                                localPlayer->_velocity.y *= 1.1f;
                            }
                        }
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }


    void flyHackLoop() {
        while (Globals::cheatIsEnabled) {
            if (Globals::gameManager && Globals::Settings::FlyHack::isEnabled) {
                Entity* localPlayer = Globals::gameManager->_localPlayer;
                if (localPlayer) {
                    if ((GetAsyncKeyState('F') & 0x0001)) {
                        localPlayer->_physicalState == 4 ? localPlayer->_physicalState = 0 : localPlayer->_physicalState = 4;
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

namespace Movement {
    void start() {
        _speedHackThread = std::thread(speedHackLoop);
        _flyHackThread = std::thread(flyHackLoop);
    }

    void shutdown() {
        std::cout << "shutdown Movement!\n";
        if (_speedHackThread.joinable() && _flyHackThread.joinable()) {
            Globals::Settings::SpeedHack::isEnabled = false;
            Globals::Settings::FlyHack::isEnabled = false;
            _speedHackThread.join();
            _flyHackThread.join();
        }
    }
}

