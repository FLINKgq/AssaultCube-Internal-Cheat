#pragma once
#include <windows.h>
#include <memory>
#include <vector>

class Entity;
class GameManager;

namespace Globals {
	inline bool cheatIsEnabled = false;
	inline GameManager* gameManager = nullptr;
	
	void init();

	namespace Addresses{
		inline uintptr_t baseAddress = 0;
	
		namespace Offssets {
			inline uintptr_t viewMatrix = 0;
			inline uintptr_t recoilConst = 0;
			inline uintptr_t noSpread = 0;
		}

		namespace FunctionAddresses {
			inline uintptr_t rayDamage = 0;
			inline uintptr_t attackPhysics = 0;
		}
	}

	namespace Settings {

		namespace Aimbot {
			inline std::atomic<bool> isEnabled = false;
			inline float fov = 15.0f;
			inline std::atomic<bool> friendlyFireOn = false;
			inline float smoothness = 1.0f;
		}

		namespace Menu {
			inline bool draw = false;
		}

		namespace Esp {
			inline bool drawCircle = false;
			inline bool isEnabled = false;
			inline bool drawNames = false;
			inline std::vector<int> sideLeft = { 0 }; 
			inline std::vector<int> sideRight = {};
			inline std::vector<int> sideTop = { 1 }; 
			inline std::vector<int> sideBottom = {};
			inline void* previewTexture = nullptr;
			inline float boxWidthMult = 1.0f;  
			inline float boxHeightMult = 1.0f; 
			inline float boxShiftY = 0.0f;    
			inline bool drawSnaplines = false; 
			inline int snaplinePosition = 0;   // 0 = bottom, 1 = center, 2 = top
		}

		namespace LittleFeatures {
			inline std::atomic<bool> infiniteAmmoOn = false;
			inline std::atomic<bool> rapidFireOn = false;
			inline std::atomic<bool> godModeOn = false;
		
		}

		namespace SpeedHack {
			inline std::atomic<bool> isEnabled = false;
			inline float maxSpeed = 1.0f;
		}

		namespace FlyHack {
			inline std::atomic<bool> isEnabled = false;
		}

		namespace SilentAim {
			inline std::atomic<bool> isEnabled = false;
			namespace Mode {
				inline std::atomic<bool> Rage = false;
				inline std::atomic<bool> ToClosestEnemy = false;
				inline std::atomic<bool> ToClosestEnemyToTheCrosshair = false; //i hate fat peo
			}
		}
		
		namespace Hooks {
			inline bool noSpreadANDRecoilOn = false;
		}
	}

	namespace Signatures {
		constexpr const char* attackPhysics = "83 EC ? 53 55 8B 6C 24 ? 56 57 8B F9 8B 45 ? F3 0F 7E 45 ? 66 0F D6 44 24 ? F3 0F 10 44 24 ? F3 0F 10 54 24 ? 89 44 24 ?";
		constexpr const char* rayDamage = "55 8B EC 83 E4 ? 81 EC ? ? ? ? 53 8B DA 89 4C 24";
	}
}