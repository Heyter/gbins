#pragma once

#include "ILuaModuleManager.h"
#include "ILuaInterface.h"
#include <stdint.h>

namespace Murmur {
	extern uint64_t MurmurHash64B(const void * key, int len, unsigned int seed);
	extern LUA_FUNCTION(MurmurHash);
}