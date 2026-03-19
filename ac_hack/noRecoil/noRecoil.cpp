#include "pch.h"
#include "noRecoil.h"
#include "../entity.h"
#include "../gameManager.h"
#include "../globals/globals.h"

namespace {
    std::thread _mainThread;

    void mainLoop() {
        while (Globals::cheatIsEnabled) {
            if (Globals::Settings::NoRecoil::isEnabled && Globals::gameManager) {
                Entity* localPlayer = Globals::gameManager->_localPlayer;
                if (localPlayer) {
                    localPlayer->_recoil = 0; //without thread sleep cuz game thread writing this shit so fast
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(Globals::Settings::NoRecoil::isEnabled ? 0 : 10)); //0 - without sleeping
        }
    }
}

namespace NoRecoil {
    void shutdown() {
        std::cout << "shutdown noRecoil!\n";
        Globals::Settings::NoRecoil::isEnabled = false;
        if (_mainThread.joinable()) {
            _mainThread.join();
        }
    }

    void start() {
        _mainThread = std::thread(mainLoop);
    }
}
