#pragma once

#include "ILuaInterface.h"
#include "ILuaModuleManager.h"
#include "LuaBase.h"
#include "Database.h"

extern void println(ILuaInterface&, const char* msg);
extern Database db;