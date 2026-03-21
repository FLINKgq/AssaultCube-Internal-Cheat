#include "globals.h"
#include "../gameManager.h"
#include "../entity.h"
#include "../cheatMemory/cheatMemory.h"
#include <memory>
#include "iostream"

namespace Globals {

	void init() {
		cheatIsEnabled = true;
		HMODULE hModule = GetModuleHandleA("ac_client.exe");
		Globals::Addresses::baseAddress = (uintptr_t)hModule;
		if (Globals::Addresses::baseAddress) {

			gameManager = (GameManager*)(Globals::Addresses::baseAddress + 0x18AB78);
			Globals::Addresses::Offssets::viewMatrix = 0x17DFD0;
			Globals::Addresses::Offssets::recoilConst = 0x15BF58;
			Globals::Addresses::Offssets::noSpread = 0xC8C51;
			
			//attackPhysics
			Globals::Addresses::FunctionAddresses::attackPhysics = Memory::SigScanner::FindPattern(hModule, Globals::Signatures::attackPhysics);
			//rayDamage
			Globals::Addresses::FunctionAddresses::rayDamage = Memory::SigScanner::FindPattern(hModule,Globals::Signatures::rayDamage);
		}
	}

}