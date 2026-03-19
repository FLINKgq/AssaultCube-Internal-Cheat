#pragma once
#include "cheatMath/vector3.h"
#include "currentWeapon.h"
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

class Entity
{
public:
	union {
		DEFINE_MEMBER_N(Vector3, _headPos, 0x0004);
		DEFINE_MEMBER_N(Vector3, _velocity, 0x0010);
		DEFINE_MEMBER_N(Vector3, _pos, 0x0028);
		DEFINE_MEMBER_N(Vector3, _angles, 0x0034);
		DEFINE_MEMBER_N(float, _recoil, 0x0040);
		DEFINE_MEMBER_N(bool, _isOnGround, 0x005D);
		DEFINE_MEMBER_N(bool, _isMovingStraight, 0x0074);
		DEFINE_MEMBER_N(int8_t, _physicalState, 0x0076); // 0 - default 4 - fly
		DEFINE_MEMBER_N(int8_t, _playerState, 0x0076); //0 - alive 1 - dead 4 - edit mode 5 - spectate mode
		DEFINE_MEMBER_N(int32_t, _health, 0x00EC);
		DEFINE_MEMBER_N(int32_t, _armor, 0x00F0);
		DEFINE_MEMBER_N(int32_t, _pistolAmmo, 0x012C);
		DEFINE_MEMBER_N(int32_t, _subgunAmmo, 0x0138);
		DEFINE_MEMBER_N(int32_t, _sniperAmmo, 0x013C);
		DEFINE_MEMBER_N(int32_t, _rifleAmmo, 0x0140);
		DEFINE_MEMBER_N(int32_t, _grenadeCount, 0x0144);
		DEFINE_MEMBER_N(int32_t, _coolDownSubgun, 0x0160);
		DEFINE_MEMBER_N(int32_t, _coolDownSniper, 0x015C);
		DEFINE_MEMBER_N(int32_t, _coolDownRiffle, 0x0164);
		DEFINE_MEMBER_N(int32_t, _coolDownGrenade, 0x0168);
		DEFINE_MEMBER_N(char, _name[16], 0x0205);
		DEFINE_MEMBER_N(int32_t, _currentTeam, 0x030C);
		DEFINE_MEMBER_N(CurrentWeapon*, _currentWeaponPtr, 0x0368);
	};
};
