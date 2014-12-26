#ifndef _MAIN_HEADER_
#define _MAIN_HEADER_

#pragma comment(linker, "/NODEFAULTLIB:libcmt") 
#define _RETAIL
#define GAME_DLL

#ifdef _WIN32
	#define WINDOWS_LEAN_AND_MEAN
	#define ENGINE_LIB "engine.dll"
#elif defined _LINUX
	#define ENGINE_LIB "engine_srv.so"
#endif

#include "MODULE_LuaOO.h"
#include "eiface.h"
#include "interface.h"
#include "tier1.h"
#include <string>
#include "GarrysMod/Lua/Interface.h"
#include <stdio.h>
#include "Lua.h"

extern GarrysMod::Lua::ILuaBase* g_Lua;
extern IVEngineServer *g_IVEngineServer;

using namespace GarrysMod::Lua;

int GetClusterCount( lua_State* state );
int GetClusterForOrigin( lua_State* state );
int GetAllClusterBounds( lua_State* state );
int GetPVSForCluster( lua_State* state );

class PVS :
	public LuaObjectBaseTemplate<PVS, 301>
{
	LUA_OBJECT
public:
	PVS(lua_State* state);
	virtual ~PVS(void);
	int CheckCluster();
	int GetList();
	int CheckOrigin();
	int CheckBox();
private:
	byte* pvs;
}; 

#endif


