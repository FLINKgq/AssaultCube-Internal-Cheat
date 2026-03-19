#include "pch.h"
#include "utils.h"

void ShowConsole() {
    if (!AllocConsole()) {
        return;
    }

    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);

    std::ios::sync_with_stdio(true);

    //bufers cleaning
    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();
    SetConsoleTitle(L"Debug Console");
}

void HideConsole() {
    FreeConsole();
}