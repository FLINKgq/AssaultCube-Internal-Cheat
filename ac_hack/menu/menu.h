#pragma once
#include "imgui/imgui.h"

class Menu {
private:
    static bool showMenu;

public:
    static void Draw();
    static void Toggle();
    static bool IsOpen();
};