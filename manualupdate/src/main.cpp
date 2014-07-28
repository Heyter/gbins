
#include <string.h>
#include <stdio.h>

#include <dlfcn.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "memutils.h"

//#include "vfnhook.h"

#include "Bootil/Bootil.h"

#include "tier0/dbg.h"


extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
}


typedef bool (*	tfunc ) ( const Bootil::BString&,const Bootil::BString&,const Bootil::BString& ) ;
tfunc original_func = NULL;

static int HandleChange_Lua(lua_State *L) {

	const char *folder = luaL_checkstring(L, 1);
	const char *file = luaL_checkstring(L, 2);
	const char *ext = luaL_checkstring(L, 3);
	
	bool ret = original_func(folder,file,ext);
	
	lua_pushboolean(L,ret);
	
	return 1; 
}

extern "C" __attribute__( ( visibility("default") ) ) int gmod13_open( lua_State* L )
{
	void *lHandle = dlopen( "garrysmod/bin/server_srv.so", RTLD_LAZY  );


	if ( lHandle )
	{
		original_func = (tfunc)ResolveSymbol( lHandle, "_ZN9GarrysMod11AutoRefresh16HandleChange_LuaERKSsS2_S2_" );
		if (original_func) {
			lua_pushcfunction(L, HandleChange_Lua);
			lua_setglobal(L, "HandleChange_Lua");
		} else {
			Warning("func: Detour failed: Signature not found. (plugin needs updating)\n");
		}

		dlclose( lHandle );
	} else 
	{
		Warning("handle failed???\n");
	}
	
	
	void *lHandle = dlopen( "garrysmod/bin/server_srv.so", RTLD_LAZY  );


	if ( lHandle )
	{
		original_func = (tfunc)ResolveSymbol( lHandle, "_ZN9GarrysMod11AutoRefresh16HandleChange_LuaERKSsS2_S2_" );
		if (original_func) {
			lua_pushcfunction(L, HandleChange_Lua);
			lua_setglobal(L, "HandleChange_Lua");
		} else {
			Warning("func: Detour failed: Signature not found. (plugin needs updating)\n");
		}

		dlclose( lHandle );
	} else 
	{
		Warning("handle failed???\n");
	}
	
	
	return 0;
}

extern "C" __attribute__( ( visibility("default") ) ) int gmod13_close( lua_State* L )
{
	return 0;
}