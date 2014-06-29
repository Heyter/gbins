#include <string.h>
#include <stdio.h>

#include <dlfcn.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "memutils.h"

#include "vfnhook.h"


#include "detours.h"

#include "tier0/dbg.h"


extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
}

#define LOG(x) write(2,x,strlen(x))

typedef bool (*	tCheckIPInternal ) ( netadr_s & ) ;
tCheckIPInternal original_CheckIPInternal = NULL;

lua_State* L = NULL;

MologieDetours::Detour<tCheckIPInternal>* detour_CheckIPInternal = NULL;

typedef bool (*	tCheckIP ) ( netadr_s & ) ;
tCheckIP original_CheckIP = NULL;
MologieDetours::Detour<tCheckIP>* detour_CheckIP = NULL;
int lastCount=0;
int rettbl=LUA_NOREF;



bool hook_CheckIPInternal( int thisptr, int currentChecksDone )
{
	int ret = 0;
	if ( currentChecksDone < 1200 )
	{
		ret = 1200 - currentChecksDone;
	}
	if (!L) return ret;
	
	lua_getglobal(L, "hook");                 		
		lua_getfield(L, -1, "Run");              	
		lua_pushstring(L,"AdditionalCollisions");  	
		lua_pushinteger(L, currentChecksDone);    	

		// do the call (2 arguments, 1 result) 		
		if (lua_pcall(L, 2, 1, 0) == 0) {
			
			if (lua_isnumber(L, -1)) {
				int checks = lua_tointeger(L, -1);
				ret = checks>0?checks:0;
			}
			
			lua_pop(L, 1); // pop result
			
		} else { // errored
			LOG("<FAIL: ");
			const char* err = lua_tostring(L, -1);
			LOG(err);
			lua_pop(L, 1); // pop error
			LOG(">\n");
		}

	lua_pop(L, 1); 
	
	return ret;
}




bool hook_CheckIP( netadr_s & addr )
{
	bool ret = false;
	if (!L) return ret;
	
	lua_getglobal(L, "hook");                 		
		lua_getfield(L, -1, "Run");              	
		lua_pushstring(L,"CheckIP");  	
		lua_pushinteger(L, objectCount);    	
		lua_rawgeti(L, LUA_REGISTRYINDEX, rettbl);
		
		int i=0;
		for (i=0;i<objectCount;i++) {
			
			lua_pushnumber(L, i+1);

			CBaseEntity* ent1 = (CBaseEntity*)pObjectList[i]->GetGameData(); // HUEHUEHUEHUE
			if (ent1!=NULL) {
				int eid = ent1->entindex();
				lua_pushnumber(L, eid);
			} else {
				lua_pushboolean(L,false);
			}
		
			lua_settable(L, -3);
		}
		
		// do the call ( 3 arguments, 1 result) 		
		if (lua_pcall(L, 3, 1, 0) == 0) {
			
			if (!lua_isnil(L, -1) && lua_isboolean(L, -1)) {
				bool freeze = lua_toboolean(L, -1);
				ret = freeze;
			}
			
			lua_pop(L, 1); // pop result
			
		} else { // errored
			LOG("<FAIL: ");
			const char* err = lua_tostring(L, -1);
			LOG(err);
			lua_pop(L, 1); // pop error
			LOG(">\n");
		}

	lua_pop(L, 1); 
	
	return ret;
}


extern "C" __attribute__( ( visibility("default") ) ) int gmod13_open( lua_State* LL )
{
	void *lHandle = dlopen( "bin/engine_srv.so", RTLD_LAZY  );

	L=LL;

	if ( lHandle )
	{
		 
 
		original_CheckIPInternal = ResolveSymbol( lHandle, "_ZN12CIPRateLimit15CheckIPInternalE8netadr_s" );
		if (original_CheckIPInternal) {
			try {
				detour_CheckIPInternal = new MologieDetours::Detour<tCheckIPInternal>(original_CheckIPInternal, hook_CheckIPInternal);
				loaded = true;
			}
			catch(MologieDetours::DetourException &e) {
				LOG("CheckIPInternal: Detour failed: Internal error?\n");
			}
		} else {
			LOG("CheckIPInternal: Detour failed: Signature not found. (plugin needs updating)\n");
		}

		
		original_CheckIP = ResolveSymbol( lHandle, "_ZN12CIPRateLimit7CheckIPE8netadr_s" );
		if (original_CheckIP) {
			try {
				detour_CheckIP = new MologieDetours::Detour<tCheckIP>(original_CheckIP, hook_CheckIP);
				loaded = true;
			}
			catch(MologieDetours::DetourException &e) {
				LOG("CheckIP: Detour failed: Internal error?\n");
			}
		} else {
			LOG("CheckIP: Detour failed: Signature not found. (plugin needs updating)\n");
		}

		dlclose( lHandle );
	} else 
	{
		LOG("handle failed???\n");
	}
	
	return 0;
}

extern "C" __attribute__( ( visibility("default") ) ) int gmod13_close( lua_State* LL )
{
	delete detour_CheckIPInternal;
	delete original_CheckIP;
	L = NULL;
	return 0;
}