#include "Menu.h"
#define NOMINMAX
#include "../globals/globals.h"
#include "../esp/esp.h"
#include "../cheatMemory/cheatMemory.h"
#include "../movement/movement.h"
#include "../littleFeatures/littleFeatures.h"
#include "../aimbot/aimbot.h"
#include "../noRecoil/noRecoil.h"
#include "../cheatMemory/cheatMemory.h"
#include <vector>
#include <string>
#include <Windows.h>
#include <algorithm>

bool Menu::showMenu = false;

void Menu::Toggle() {
    showMenu = !showMenu;
}

bool Menu::IsOpen() {
    return showMenu;
}

void Menu::Draw() {

    if (!showMenu) return;

    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Fatnigga", &showMenu)) {

        if (ImGui::BeginTabBar("Tabs")) {

            if (ImGui::BeginTabItem("Aimbot")) {
                {
                    bool tempEnabled = Globals::Settings::Aimbot::isEnabled.load();
                    if (ImGui::Checkbox("Aimbot", &tempEnabled)) {
                        Globals::Settings::Aimbot::isEnabled.store(tempEnabled);
                    }
                    ImGui::SliderFloat("Smoothness", &Globals::Settings::Aimbot::smoothness, 1.0f, 30.0f, "%.1f");
                }
                ImGui::SliderFloat("FOV", &Globals::Settings::Aimbot::fov, 0.0f, 500.0f);
                ImGui::Checkbox("Draw FOV circle", &Globals::Settings::Esp::drawCircle);

                {
                    bool tempEnabled = Globals::Settings::Aimbot::friendlyFireOn.load();
                    if (ImGui::Checkbox("Friendly fire", &tempEnabled)) {
                        Globals::Settings::Aimbot::friendlyFireOn.store(tempEnabled);
                    }
                }

                {
                    bool tempEnabled = Globals::Settings::SilentAim::isEnabled.load();
                    if (ImGui::Checkbox("Silent aim", &tempEnabled)) {
                        Globals::Settings::SilentAim::isEnabled.store(tempEnabled);
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Visuals")) {

                ImGui::Checkbox("ESP", &Globals::Settings::Esp::isEnabled);
                ImGui::ColorEdit4("Enemy ESP color", (float*)&ESP::enemyBoxColor);
                ImGui::ColorEdit4("Team ESP color", (float*)&ESP::teamBoxColor);
                ImGui::Checkbox("Draw names", &Globals::Settings::Esp::drawNames);

                ImGui::Checkbox("Draw snaplines", &Globals::Settings::Esp::drawSnaplines);
                if (Globals::Settings::Esp::drawSnaplines) {
                    //if lines are enabled, show dropdown list to select position
                    ImGui::Combo("Snapline Start", &Globals::Settings::Esp::snaplinePosition, "Bottom\0Center\0Top\0");
                }
                ImGui::Separator();
                ImGui::Text("Box adjustments:");
                ImGui::SliderFloat("Box width", &Globals::Settings::Esp::boxWidthMult, 0.1f, 3.0f, "%.2fx");
                ImGui::SliderFloat("Box height", &Globals::Settings::Esp::boxHeightMult, 0.1f, 3.0f, "%.2fx");
                ImGui::SliderFloat("Box Y Offset", &Globals::Settings::Esp::boxShiftY, -1.5f, 1.5f, "%.2f");

                ImGui::Separator();
                ImGui::Text("Drag elements to layout them");

                //--- advanced slot system editor ---
                float previewWidth = 250.0f;
                float previewHeight = 250.0f;
                ImVec2 p = ImGui::GetCursorScreenPos();
                ImDrawList* drawList = ImGui::GetWindowDrawList();

                drawList->AddRectFilled(p, ImVec2(p.x + previewWidth, p.y + previewHeight), IM_COL32(40, 40, 40, 255));
                drawList->AddRect(p, ImVec2(p.x + previewWidth, p.y + previewHeight), IM_COL32(100, 100, 100, 255));

                ImVec2 center = ImVec2(p.x + previewWidth / 2.0f, p.y + previewHeight / 2.0f);

                //--- 1. static base (the picture/character itself) ---
                //these dimensions are not changed by sliders, the picture is always in place
                float baseW = 80.0f;
                float baseH = 160.0f;
                ImVec2 baseMin(center.x - baseW / 2.0f, center.y - baseH / 2.0f);
                ImVec2 baseMax(center.x + baseW / 2.0f, center.y + baseH / 2.0f);

                //drawing model preview (statically)
                if (Globals::Settings::Esp::previewTexture != nullptr) {
                    drawList->AddImage((ImTextureID)(intptr_t)Globals::Settings::Esp::previewTexture, baseMin, baseMax);
                }
                else {
                    //draw dummy if there is no texture (also strictly centered)
                    drawList->AddRectFilled(ImVec2(center.x - 20, center.y - 30), ImVec2(center.x + 20, center.y + 60), IM_COL32(150, 150, 150, 255));
                    drawList->AddCircleFilled(ImVec2(center.x, center.y - 45), 12.0f, IM_COL32(200, 200, 200, 255));
                }

                //esp box
                float boxW = baseW;
                float boxH = baseH;
                float cx = center.x;
                float cy = center.y;

                //coordinates for box and elements (hp, name)
                ImVec2 boxMin(cx - boxW / 2.0f, cy - boxH / 2.0f);
                ImVec2 boxMax(cx + boxW / 2.0f, cy + boxH / 2.0f);

                //draw stretching esp box over the static picture
                drawList->AddRect(boxMin, boxMax, IM_COL32(255, 255, 255, 100), 0.0f, 0, 1.0f);

                std::vector<int>* sides[4] = {
                    &Globals::Settings::Esp::sideLeft,
                    &Globals::Settings::Esp::sideRight,
                    &Globals::Settings::Esp::sideTop,
                    &Globals::Settings::Esp::sideBottom
                };

                static int draggedId = -1;
                static int pendingDropId = -1;
                float padding = 4.0f;

                for (int s = 0; s < 4; s++) {
                    float offset = 3.0f;
                    for (int id : *sides[s]) {

                        ImVec2 eMin, eSize;

                        //parameters for health
                        if (id == 0) {
                            float thick = 6.0f;
                            if (s == 0) { eMin = ImVec2(boxMin.x - offset - thick, boxMin.y); eSize = ImVec2(thick, boxH); }
                            if (s == 1) { eMin = ImVec2(boxMax.x + offset, boxMin.y);         eSize = ImVec2(thick, boxH); }
                            if (s == 2) { eMin = ImVec2(boxMin.x, boxMin.y - offset - thick); eSize = ImVec2(boxW, thick); }
                            if (s == 3) { eMin = ImVec2(boxMin.x, boxMax.y + offset);         eSize = ImVec2(boxW, thick); }
                        }
                        //parameters for name
                        else if (id == 1) {
                            ImVec2 tSize = ImGui::CalcTextSize("Name");
                            eSize = tSize;
                            if (s == 0) { eMin = ImVec2(boxMin.x - offset - tSize.x, cy - tSize.y / 2); }
                            if (s == 1) { eMin = ImVec2(boxMax.x + offset, cy - tSize.y / 2); }
                            if (s == 2) { eMin = ImVec2(cx - tSize.x / 2, boxMin.y - offset - tSize.y); }
                            if (s == 3) { eMin = ImVec2(cx - tSize.x / 2, boxMax.y + offset); }
                        }

                        //always create an invisible button so imgui doesn't lose focus when dragging
                        ImGui::SetCursorScreenPos(eMin);
                        ImGui::InvisibleButton((std::string("drag_") + std::to_string(id)).c_str(), eSize);

                        bool isHovered = ImGui::IsItemHovered();
                        bool isActive = ImGui::IsItemActive();

                        //grab logic
                        if (isActive) {
                            draggedId = id;
                        }
                        else if (draggedId == id) {
                            pendingDropId = id; //mouse released -> trigger drop
                            draggedId = -1;
                        }

                        //draw element at its usual place only if we are not dragging it right now
                        if (draggedId != id) {
                            ImU32 colState = isHovered ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 255, 255, 255);

                            if (id == 0) {
                                drawList->AddRectFilled(eMin, ImVec2(eMin.x + eSize.x, eMin.y + eSize.y), colState == IM_COL32(255, 255, 255, 255) ? IM_COL32(0, 255, 0, 255) : colState);
                                drawList->AddRect(eMin, ImVec2(eMin.x + eSize.x, eMin.y + eSize.y), IM_COL32(0, 0, 0, 255));
                            }
                            else if (id == 1) {
                                drawList->AddText(eMin, colState, "Name");
                            }
                        }

                        offset += (s < 2) ? (eSize.x + padding) : (eSize.y + padding);
                    }
                }

                //drawing ghost attached to mouse
                if (draggedId != -1) {
                    ImVec2 mPos = ImGui::GetIO().MousePos;
                    if (draggedId == 0) drawList->AddRectFilled(mPos, ImVec2(mPos.x + 6.0f, mPos.y + 40.0f), IM_COL32(255, 255, 0, 200));
                    if (draggedId == 1) drawList->AddText(mPos, IM_COL32(255, 255, 0, 255), "Name");
                }

                //drop logic (calculating which side and layer it was dropped on)
                if (pendingDropId != -1) {
                    ImVec2 mouse = ImGui::GetIO().MousePos;
                    float dx = mouse.x - cx;
                    float dy = mouse.y - cy;

                    //1. calculating side
                    int targetSide = 0;
                    if (std::abs(dx) > std::abs(dy)) targetSide = (dx < 0) ? 0 : 1;
                    else targetSide = (dy < 0) ? 2 : 3;

                    //2. calculating distance to understand the layer
                    float dist = 0;
                    if (targetSide == 0) dist = boxMin.x - mouse.x;
                    if (targetSide == 1) dist = mouse.x - boxMax.x;
                    if (targetSide == 2) dist = boxMin.y - mouse.y;
                    if (targetSide == 3) dist = mouse.y - boxMax.y;

                    int insertIdx = 0;
                    float curOff = 0;
                    auto& tVec = *sides[targetSide];

                    for (size_t k = 0; k < tVec.size(); k++) {
                        if (tVec[k] == pendingDropId) continue;
                        float thick = (tVec[k] == 0) ? 6.0f : ((targetSide < 2) ? 40.0f : 15.0f);
                        if (dist > curOff + thick / 2.0f) insertIdx++;
                        curOff += thick + padding;
                    }

                    //3. remove element from everywhere and insert into the right place
                    for (int s = 0; s < 4; s++) {
                        auto& v = *sides[s];
                        v.erase(std::remove(v.begin(), v.end(), pendingDropId), v.end());
                    }

                    if (insertIdx > tVec.size()) insertIdx = tVec.size();
                    tVec.insert(tVec.begin() + insertIdx, pendingDropId);

                    pendingDropId = -1;
                }

                ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + previewHeight + 10.0f));

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Weapon")) {
                {
                    bool tempEnabled = Globals::Settings::NoRecoil::isEnabled.load();
                    if (ImGui::Checkbox("No recoil", &tempEnabled)) {
                        Globals::Settings::NoRecoil::isEnabled.store(tempEnabled);
                        float newRecoilConst = Globals::Settings::NoRecoil::isEnabled ? 0.0f : -0.0099999998f;
                        Memory::patchds((Globals::Addresses::baseAddress + Globals::Addresses::Offssets::recoilConst), &newRecoilConst, sizeof(float));
                    }
                }

                {
                    bool tempEnabled = Globals::Settings::LittleFeatures::infiniteAmmoOn.load();
                    if (ImGui::Checkbox("Infinite ammo", &tempEnabled)) {
                        Globals::Settings::LittleFeatures::infiniteAmmoOn.store(tempEnabled);
                    }
                }

                {
                    bool tempEnabled = Globals::Settings::LittleFeatures::rapidFireOn.load();
                    if (ImGui::Checkbox("Rapid fire", &tempEnabled)) {
                        Globals::Settings::LittleFeatures::rapidFireOn.store(tempEnabled);
                    }
                }

                {
                    if (ImGui::Checkbox("No spread", &Globals::Settings::LittleFeatures::noSpreadOn)) {
                        Memory::patchNoSpread();
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Movement")) {

                {
                    bool tempEnabled = Globals::Settings::SpeedHack::isEnabled.load();
                    if (ImGui::Checkbox("Speed hack", &tempEnabled)) {
                        Globals::Settings::SpeedHack::isEnabled.store(tempEnabled);
                    }
                    ImGui::SliderFloat("Max speed", &Globals::Settings::SpeedHack::maxSpeed, 1.0f, 5.0f);
                }

                {
                    bool tempEnabled = Globals::Settings::FlyHack::isEnabled.load();
                    if (ImGui::Checkbox("Fly hack (press F to fly)", &tempEnabled)) {
                        Globals::Settings::FlyHack::isEnabled.store(tempEnabled);
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Misc")) {

                {
                    bool tempEnabled = Globals::Settings::LittleFeatures::godModeOn.load();
                    if (ImGui::Checkbox("God mode", &tempEnabled)) {
                        Globals::Settings::LittleFeatures::godModeOn.store(tempEnabled);
                    }
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}