#pragma once
#include <windows.h>

namespace Core {
	 void init(void* hModule);
	 DWORD WINAPI mainLoop(LPVOID lpParam);
}