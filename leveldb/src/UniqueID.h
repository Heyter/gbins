#pragma once
#include "ILuaModuleManager.h"
#include <stdint.h>

namespace UniqueID {
	extern void initLua(ILuaInterface& g_Lua);
	extern LUA_FUNCTION(getUniqueID);
}