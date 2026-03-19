#include "globals.h"
#include "../gameManager.h"
#include "../entity.h"
#include <memory>

//TODO move to signatures

namespace Globals {

	void init() {
		cheatIsEnabled = true;
		Globals::Addresses::baseAddress = (uintptr_t)GetModuleHandleA("ac_client.exe");
		if (Globals::Addresses::baseAddress) {
			gameManager = (GameManager*)(Globals::Addresses::baseAddress + 0x18AB78);
			Globals::Addresses::FunctionOffsets::rayDamage = 0xC9510;
			Globals::Addresses::Offssets::viewMatrix = 0x17DFD0;
			Globals::Addresses::Offssets::recoilConst = 0x15BF58;
			Globals::Addresses::Offssets::noSpread = 0xC8C51;
		}
	}

}