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

#include "studio.h"
#include "cmodel.h"
#include "bitvec.h"

#include "tier0/dbg.h"

#define GAME_DLL 1
#include "cbase.h"
#include "vphysics_interface.h"

#include <GarrysMod/Lua/Interface.h>

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
}

// memdbgon must be the last include file in a .cpp file!!!
//#include "tier0/memdbgon.h"


lua_State* L = NULL;

CEntList* pEntList;

typedef bool (*	tBombIsFriend ) ( CBaseEntity * , CBaseEntity *   ) ;
tBombIsFriend original_BombIsFriend = NULL;
MologieDetours::Detour<tBombIsFriend>* detour_BombIsFriend = NULL;


bool hook_BombIsFriend( CBaseEntity * bomb, CBaseEntity * ent )
{
	bool ret = false;
	if (!L) return ret;
	
	lua_getglobal(L, "hook");                 		
		lua_getfield(L, -1, "Run");              	
		
		lua_pushstring(L,"EntLikesEnt");
		lua_pushnumber(L, bomb->entindex());
		lua_pushnumber(L, ent->entindex());
		
		
		// do the call ( 3 arguments, 1 result) 		
		if (lua_pcall(L, 3, 1, 0) == 0) {
			
			if (!lua_isnil(L, -1) && lua_isboolean(L, -1)) {
				bool isfriend = lua_toboolean(L, -1);
				ret = isfriend;
			}
			
			lua_pop(L, 1); // pop result
			
		} else { // errored
			const char* err = lua_tostring(L, -1);
			Warning("Eek: %s\n",err);
			lua_pop(L, 1); // pop error
		}

	lua_pop(L, 1); 
	
	return ret;
}



typedef float (*	tFindNearestNPC ) ( CBaseEntity *  ) ;
tFindNearestNPC original_FindNearestNPC = NULL;
MologieDetours::Detour<tFindNearestNPC>* detour_FindNearestNPC = NULL;


float hook_FindNearestNPC( CBaseEntity * bomb )
{
	bool ret = false;
	if (!L) return ret;
	
	lua_getglobal(L, "hook");                 		
		lua_getfield(L, -1, "Run");              	
		
		lua_pushstring(L,"CombineNearestNPC");
		lua_pushnumber(L, bomb->entindex());
		
		
		// do the call ( 3 arguments, 1 result) 		
		if (lua_pcall(L, 2, 2, 0) == 0) {
			
			if (!LUA->IsType( -1, GarrysMod::Lua::Type::ENTITY ) ||
				!LUA->IsType( -2, GarrysMod::Lua::Type::NUMBER ) ||)
			{
				return detour_FindNearestNPC->GetOriginalFunction()( bomb );
			}
			
			float dist = lua_tonumber(L, -1);
			GarrysMod::Lua::UserData* Userdata = (GarrysMod::Lua::UserData*) LUA->GetUserdata( -2 );
			CBaseEntity* pEnt = pEntList->GetClientEntityFromHandle( *(int*)Userdata->data );
			
			if (!lua_isnil(L, -1) && lua_isboolean(L, -1)) {
				ret = isfriend;
				
				if (!lua_isnil(L, -1) && lua_isboolean(L, -1)) {
					bool isfriend = lua_toboolean(L, -1);
					ret = isfriend;
				}
				
			}
			
			lua_pop(L, 1); // pop result
			
		} else { // errored
			const char* err = lua_tostring(L, -1);
			Warning("Eek2: %s\n",err);
			lua_pop(L, 1); // pop error
		}

	lua_pop(L, 1); 
	
	return ret;
}


extern "C" __attribute__( ( visibility("default") ) ) int gmod13_open( lua_State* LL )
{
	void *lHandle = dlopen( "garrysmod/bin/server_srv.so", RTLD_LAZY  );


	CreateInterfaceFn ClientFactory = Sys_GetFactory( "server_srv.so" );
	pEntList = (CEntList*) ClientFactory( "VClientEntityList003", NULL );
	if (!pEntList) {
		Warning("pEntList: Not found?\n");
		return 0;
	}
	L=LL;
		
	if ( lHandle )
	{
		
		original_FindNearestNPC = ResolveSymbol( lHandle, "_ZN11CBounceBomb8IsFriendEP11CBaseEntity" );
		if (original_FindNearestNPC) {
			try {
				detour_FindNearestNPC = new MologieDetours::Detour<tFindNearestNPC>(original_FindNearestNPC, hook_FindNearestNPC);
			}
			catch(MologieDetours::DetourException &e) {
				Warning("FindNearestNPC: Detour failed: Internal error?\n");
			}
		} else {
			Warning("FindNearestNPC: Detour failed: Signature not found. (plugin needs updating)\n");
		}

		
		original_BombIsFriend = ResolveSymbol( lHandle, "_ZN11CBounceBomb8IsFriendEP11CBaseEntity" );
		if (original_BombIsFriend) {
			try {
				detour_BombIsFriend = new MologieDetours::Detour<tBombIsFriend>(original_BombIsFriend, hook_BombIsFriend);
			}
			catch(MologieDetours::DetourException &e) {
				Warning("BombIsFriend: Detour failed: Internal error?\n");
			}
		} else {
			Warning("BombIsFriend: Detour failed: Signature not found. (plugin needs updating)\n");
		}

		dlclose( lHandle );
	} else {
		Warning("BombIsFriend: Detour failed. File changed?\n");
	}
	
	return 0;
}

extern "C" __attribute__( ( visibility("default") ) ) int gmod13_close( lua_State* LL )
{
	if ( detour_BombIsFriend ) delete detour_BombIsFriend;
	if ( detour_FindNearestNPC ) delete detour_FindNearestNPC;
	L = NULL;
	return 0;
}