#include "esp.h"
#define NOMINMAX
#include <iostream>
#include "../globals/globals.h"
#include "../imgui/imgui.h"
#include "../gameManager.h"
#include "../entity.h"
#include "../cheatMath/cheatMath.h"
#include <cmath>
#define PI 3.14159265358979323846f
#include <algorithm>
#include <vector>

namespace ESP {
    void Draw() {
        //getting pointers to local player, entity list and view matrix
        Entity* localPlayer = Globals::gameManager->_localPlayer;
        Entity** entityList = (Entity**)Globals::gameManager->_entities;
        float* viewMatrix = (float*)(Globals::Addresses::baseAddress + Globals::Addresses::Offssets::viewMatrix);

        //sanity checks to prevent crashes
        if (!localPlayer || !entityList || Globals::gameManager->_playersCounter <= 0) {
            return;
        }

        //getting screen size and imgui drawlist
        ImGuiIO& io = ImGui::GetIO();
        int screenWidth = (int)io.DisplaySize.x;
        int screenHeight = (int)io.DisplaySize.y;
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        //looping through all entities in the game
        for (int i = 1; i < Globals::gameManager->_playersCounter; i++) {
            Entity* enemy = entityList[i];

            //skipping dead, invalid or local entities
            if (!enemy || enemy->_health <= 0 || enemy == localPlayer) continue;

            Vector3 enemyHead = enemy->_headPos;
            Vector3 enemyFeet = enemy->_pos;
            enemyHead.z += 1.0f;

            Vector2 screenFeet, screenHead;

            //converting 3d world coordinates to 2d screen space
            if (WorldToScreen(enemyFeet, screenFeet, viewMatrix, screenWidth, screenHeight) &&
                WorldToScreen(enemyHead, screenHead, viewMatrix, screenWidth, screenHeight)) {

                //calculating box dimensions and position
                float baseMinY = std::min(screenHead.y, screenFeet.y);
                float baseMaxY = std::max(screenHead.y, screenFeet.y);
                float baseHeight = baseMaxY - baseMinY;

                float height = baseHeight * Globals::Settings::Esp::boxHeightMult;
                float width = (baseHeight / 2.2f) * Globals::Settings::Esp::boxWidthMult;

                float cx = (screenHead.x + screenFeet.x) / 2.0f;
                float baseCy = baseMinY + baseHeight / 2.0f;
                float cy = baseCy + (baseHeight * Globals::Settings::Esp::boxShiftY);

                ImVec2 boxMin(cx - width / 2.0f, cy - height / 2.0f);
                ImVec2 boxMax(cx + width / 2.0f, cy + height / 2.0f);

                //drawing the main esp bounding box
                ImColor boxColor = (enemy->_currentTeam == localPlayer->_currentTeam) ? teamBoxColor : enemyBoxColor;
                drawList->AddRect(boxMin, boxMax, boxColor, 0.0f, 0, 1.5f);

                //snaplines drawing logic
                if (Globals::Settings::Esp::drawSnaplines) {
                    ImVec2 startPos;

                    //calculating start point on screen depending on settings
                    if (Globals::Settings::Esp::snaplinePosition == 0) {
                        //bottom center of screen
                        startPos = ImVec2(screenWidth / 2.0f, screenHeight);
                    }
                    else if (Globals::Settings::Esp::snaplinePosition == 1) {
                        //exact center of screen (from crosshair)
                        startPos = ImVec2(screenWidth / 2.0f, screenHeight / 2.0f);
                    }
                    else {
                        //top center of screen
                        startPos = ImVec2(screenWidth / 2.0f, 0.0f);
                    }

                    //target point is center of enemy box bottom edge (feet)
                    ImVec2 targetPos = ImVec2(cx, boxMax.y);

                    //drawing the line itself (color taken from box)
                    drawList->AddLine(startPos, targetPos, boxColor, 1.0f);
                }

                //--- esp elements slot system ---
                //setting up array of pointers to side settings
                std::vector<int>* sides[4] = {
                    &Globals::Settings::Esp::sideLeft,
                    &Globals::Settings::Esp::sideRight,
                    &Globals::Settings::Esp::sideTop,
                    &Globals::Settings::Esp::sideBottom
                };

                float padding = 4.0f; //padding between elements (hp, name, weapon etc)

                //looping through all four sides of the esp box
                for (int s = 0; s < 4; s++) {
                    float offset = 3.0f; //starting offset from box frame

                    for (int id : *sides[s]) {

                        //[0] - health bar
                        //rendering health bar logic
                        if (id == 0) {
                            float hpPercent = std::clamp(enemy->_health / 100.0f, 0.0f, 1.0f);
                            ImColor hpColor = ImColor((int)((1.0f - hpPercent) * 255), (int)(hpPercent * 255), 0, 255);
                            float thick = 4.0f; //bar thickness

                            if (s == 0 || s == 1) { //left or right
                                float startX = (s == 0) ? (boxMin.x - offset - thick) : (boxMax.x + offset);
                                drawList->AddRectFilled(ImVec2(startX - 1, boxMin.y - 1), ImVec2(startX + thick + 1, boxMax.y + 1), IM_COL32(0, 0, 0, 255));
                                drawList->AddRectFilled(ImVec2(startX, boxMax.y - (height * hpPercent)), ImVec2(startX + thick, boxMax.y), hpColor);
                                offset += thick + padding;
                            }
                            else { //top or bottom
                                float startY = (s == 2) ? (boxMin.y - offset - thick) : (boxMax.y + offset);
                                drawList->AddRectFilled(ImVec2(boxMin.x - 1, startY - 1), ImVec2(boxMax.x + 1, startY + thick + 1), IM_COL32(0, 0, 0, 255));
                                drawList->AddRectFilled(ImVec2(boxMin.x, startY), ImVec2(boxMin.x + (width * hpPercent), startY + thick), hpColor);
                                offset += thick + padding;
                            }
                        }

                        //[1] - player name
                        //rendering player name text
                        else if (id == 1 && Globals::Settings::Esp::drawNames) {
                            const char* playerName = enemy->_name;
                            if (playerName && playerName[0] != '\0') {
                                ImVec2 tSize = ImGui::CalcTextSize(playerName);
                                ImVec2 tPos;

                                //calculating position and centering along axis
                                if (s == 0) tPos = ImVec2(boxMin.x - offset - tSize.x, cy - tSize.y / 2.0f); //left
                                if (s == 1) tPos = ImVec2(boxMax.x + offset, cy - tSize.y / 2.0f);           //right
                                if (s == 2) tPos = ImVec2(cx - tSize.x / 2.0f, boxMin.y - offset - tSize.y); //top
                                if (s == 3) tPos = ImVec2(cx - tSize.x / 2.0f, boxMax.y + offset);           //bottom

                                //text outline
                                drawList->AddText(ImVec2(tPos.x + 1, tPos.y + 1), IM_COL32(0, 0, 0, 255), playerName);
                                drawList->AddText(ImVec2(tPos.x - 1, tPos.y - 1), IM_COL32(0, 0, 0, 255), playerName);
                                drawList->AddText(ImVec2(tPos.x + 1, tPos.y - 1), IM_COL32(0, 0, 0, 255), playerName);
                                drawList->AddText(ImVec2(tPos.x - 1, tPos.y + 1), IM_COL32(0, 0, 0, 255), playerName);

                                ImColor textColor = ImColor(255, 255, 255, 255);
                                drawList->AddText(tPos, textColor, playerName);

                                offset += (s < 2) ? (tSize.x + padding) : (tSize.y + padding);
                            }
                        }
                        //in the future you can easily add weapon here like: else if (id == 2) { ... }
                    }
                }
            }
        }
    }

    //drawing aimbot fov circle at the center of screen
    void DrawFovCircle() {
        ImGuiIO& io = ImGui::GetIO();
        float screenWidth = io.DisplaySize.x;
        float screenHeight = io.DisplaySize.y;
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        ImVec2 screenCenter = ImVec2(screenWidth / 2.0f, screenHeight / 2.0f);
        drawList->AddCircle(screenCenter, Globals::Settings::Aimbot::fov, IM_COL32(255, 255, 255, 255), 64, 1.0f);
    }
}