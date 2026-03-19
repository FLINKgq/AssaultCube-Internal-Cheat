#pragma once
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}
#include "cstdint"

class GameManager
{
public:
	union {
		DEFINE_MEMBER_N(class Entity*, _localPlayer, 0x0088);
		DEFINE_MEMBER_N(void*, _entities, 0x008C);
		DEFINE_MEMBER_N(int32_t, _playersCounter, 0x0094);
		DEFINE_MEMBER_N(int32_t, id, 0x0004);
	};
};