#pragma once
#include "../imgui/imgui.h"
namespace ESP {
    void Draw();
    void DrawFovCircle();
    inline ImColor enemyBoxColor = ImColor(255, 0, 0, 255);//red as default
    inline ImColor teamBoxColor = ImColor(0, 0, 255, 255); //blue as default
    inline float gameFov = 90.0f;
}