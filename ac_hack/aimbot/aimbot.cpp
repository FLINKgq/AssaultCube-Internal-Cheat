#include "pch.h"
#include "../entity.h"
#include "../gameManager.h"
#include "../cheatMath/cheatMath.h"
#include "../globals/globals.h"
#include "../imgui/imgui.h"
#include "aimbot.h"
#include <chrono>

namespace {
    std::thread _mainThread;

    float GetFov(Vector2 screenPos, float screenWidth, float screenHeight) {
        //display center
        float centerX = screenWidth / 2.0f;
        float centerY = screenHeight / 2.0f;
        float deltaX = screenPos.x - centerX;
        float deltaY = screenPos.y - centerY;

        return sqrt((deltaX * deltaX) + (deltaY * deltaY));
    }


    void NormalizeAngles(Vector3& angles) {
        //pitch
        if (angles.y > 89.0f) angles.y = 89.0f;
        if (angles.y < -89.0f) angles.y = -89.0f;
        //yaw
        while (angles.x < 0.0f) angles.x += 360.0f;
        while (angles.x >= 360.0f) angles.x -= 360.0f;
    }

    Entity* getClosestEntity() {
        Entity* closestEnt = nullptr;
        float bestFov = Globals::Settings::Aimbot::fov;
        Entity* localPlayer = Globals::gameManager->_localPlayer;
        Entity** entityList = (Entity**)Globals::gameManager->_entities;

        if (!localPlayer || !entityList) return nullptr;

        float* viewMatrix = (float*)(Globals::Addresses::baseAddress + Globals::Addresses::Offssets::viewMatrix);

        //get games window size
        ImGuiIO& io = ImGui::GetIO();
        float screenWidth = io.DisplaySize.x;
        float screenHeight = io.DisplaySize.y;

        for (int i = 1; i < Globals::gameManager->_playersCounter; i++) {
            Entity* currentEnt = entityList[i];

            if (!currentEnt
                || currentEnt->_playerState != 0
                || currentEnt->_health <= 0
                || currentEnt == localPlayer) continue;

            if (!Globals::Settings::Aimbot::friendlyFireOn
                && localPlayer->_currentTeam == currentEnt->_currentTeam) continue;

            Vector2 screenPos;
            if (WorldToScreen(currentEnt->_headPos, screenPos, viewMatrix, screenWidth, screenHeight)) {
                //distance in pixels between crosshair and enemy
                float fov = GetFov(screenPos, screenWidth, screenHeight);
                if (fov < bestFov) {
                    bestFov = fov;
                    closestEnt = currentEnt;
                }
            }
        }

        return closestEnt;
    }

    void mainLoop() {
        while (Globals::cheatIsEnabled) {

            if (Globals::gameManager &&
                Globals::gameManager->_playersCounter > 0 &&
                Globals::Settings::Aimbot::isEnabled) {
                Entity* localPlayer = Globals::gameManager->_localPlayer;

                if (localPlayer && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                    Entity* closestEntity = getClosestEntity();

                    if (closestEntity) {
                        Vector3 localPlayerHead = localPlayer->_headPos;
                        Vector3 targetHead = closestEntity->_headPos;

                        //calc angles to enemy
                        Vector3 aimAngles = CalcAngle(localPlayerHead, targetHead);

                        //get current player angles
                        Vector3 currentAngles = localPlayer->_angles;

                        //calculate delta angles
                        Vector3 delta;
                        delta.x = aimAngles.x - currentAngles.x;
                        delta.y = aimAngles.y - currentAngles.y;
                        delta.z = 0.0f;

                        //normalize delta yaw to find shortest path
                        while (delta.x > 180.0f) delta.x -= 360.0f;
                        while (delta.x < -180.0f) delta.x += 360.0f;

                        //apply smoothing
                        if (Globals::Settings::Aimbot::smoothness > 1.0f) {
                            delta.x /= Globals::Settings::Aimbot::smoothness;
                            delta.y /= Globals::Settings::Aimbot::smoothness;
                        }

                        //calculate final angles
                        Vector3 finalAngles;
                        finalAngles.x = currentAngles.x + delta.x;
                        finalAngles.y = currentAngles.y + delta.y;
                        finalAngles.z = 0.0f;

                        NormalizeAngles(finalAngles);
                        localPlayer->_angles = finalAngles;
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

}

namespace Aimbot {
    void shutdown() {
        std::cout << "shutdown Aimbot!\n";
        Globals::Settings::Aimbot::isEnabled = false;
        if (_mainThread.joinable()) {
            _mainThread.join();
        }
    }
    void start() {
        _mainThread = std::thread(mainLoop);
    }
}