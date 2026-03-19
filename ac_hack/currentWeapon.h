#pragma once
#include <cstdint>
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}
class Entity;
class CurrentWeapon
{
public:
	union {
		DEFINE_MEMBER_N(Entity*, _ownerPtr, 0x0008);
		DEFINE_MEMBER_N(int32_t*, _coolDown, 0x00018);
		DEFINE_MEMBER_N(char*, _name, 0x000C);
		DEFINE_MEMBER_N(int32_t*, _ammoPtr, 0x0014);
		DEFINE_MEMBER_N(int32_t, _shotsInRow, 0x001C);
		DEFINE_MEMBER_N(bool, _isReloading, 0x0020);
		DEFINE_MEMBER_N(int32_t, id, 0x0004);
	};
};
