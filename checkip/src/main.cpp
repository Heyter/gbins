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

typedef int (*	tCollcheck ) (  int , int  ) ;
tCollcheck original_AdditionalCollisionChecksThisTick = NULL;

lua_State* L = NULL;

MologieDetours::Detour<tCollcheck>* detour_AdditionalCollisionChecksThisTick = NULL;

int hook_AdditionalCollisionChecksThisTick( int thisptr, int currentChecksDone )
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

#if 0
//AdditionalCollisions
lua_run require'penicillin'
lua_run physenv.SetPerformanceSettings{ MaxCollisionChecksPerTimestep = 500 }
lua_run function frz() print"freezing all" for k,v in next,ents.GetAll() do local g=v:GetPhysicsObject() if g:IsValid() then g:EnableMotion(false) end end  end
lua_run function pen() print"freezing penetrating" for k,v in next,ents.GetAll() do local g=v:GetPhysicsObject() if g:IsValid() and g:IsPenetrating() then g:EnableMotion(false) end end  end
lua_run hook.Add("AdditionalCollisions","a",function(cc) if cc > 1000 then frz() return 0 elseif cc>500 then pen() return 100 else print"extra" return 500 end end)

lua_run hook.Add("AdditionalCollisions","a",function(a) error"nno" end)
lua_run hook.Add("AdditionalCollisions","a",function(a) return 0 end)
lua_run hook.Add("AdditionalCollisions","a",function(a) print("AdditionalCollisions",a) end)
lua_run hook.Add("AdditionalCollisions","a",function(a) for k,v in next,ents.GetAll() do local g=v:GetPhysicsObject() if g:IsValid() and g:IsPenetrating() then g:EnableMotion(false) end end end)
lua_run hook.Add("AdditionalCollisions","a",function(a) print"FRZ" end)

#endif



typedef bool (*	tFreezeCont ) ( void * , void ** , int   ) ;
tFreezeCont original_ShouldFreezeContacts = NULL;
MologieDetours::Detour<tFreezeCont>* detour_ShouldFreezeContacts = NULL;
int lastCount=0;
int rettbl=LUA_NOREF;

bool hook_ShouldFreezeContacts( void * thisptr, IPhysicsObject ** pObjectList, int objectCount )
{
	bool ret = false;
	if (!L) return ret;
	
	lua_getglobal(L, "hook");                 		
		lua_getfield(L, -1, "Run");              	
		lua_pushstring(L,"ShouldFreezeContacts");  	
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


static bool loaded = false;
extern "C" __attribute__( ( visibility("default") ) ) int gmod13_open( lua_State* LL )
{
	void *lHandle = dlopen( "garrysmod/bin/server_srv.so", RTLD_LAZY  );

	L=LL;
	lua_newtable(L);
	rettbl = luaL_ref(L, LUA_REGISTRYINDEX);
	
	if (loaded) return 0;
	
	if ( lHandle )
	{
		
		original_AdditionalCollisionChecksThisTick = ResolveSymbol( lHandle, "_ZN15CCollisionEvent33AdditionalCollisionChecksThisTickEi" );
		if (original_AdditionalCollisionChecksThisTick) {
			try {
				detour_AdditionalCollisionChecksThisTick = new MologieDetours::Detour<tCollcheck>(original_AdditionalCollisionChecksThisTick, hook_AdditionalCollisionChecksThisTick);
				loaded = true;
			}
			catch(MologieDetours::DetourException &e) {
				LOG("AdditionalCollisionChecksThisTick: Detour failed: Internal error?\n");
			}
		} else {
			LOG("AdditionalCollisionChecksThisTick: Detour failed: Signature not found. (plugin needs updating)\n");
		}

		
		original_ShouldFreezeContacts = ResolveSymbol( lHandle, "_ZN15CCollisionEvent20ShouldFreezeContactsEPP14IPhysicsObjecti" );
		if (original_ShouldFreezeContacts) {
			try {
				detour_ShouldFreezeContacts = new MologieDetours::Detour<tFreezeCont>(original_ShouldFreezeContacts, hook_ShouldFreezeContacts);
				loaded = true;
			}
			catch(MologieDetours::DetourException &e) {
				LOG("ShouldFreezeContacts: Detour failed: Internal error?\n");
			}
		} else {
			LOG("ShouldFreezeContacts: Detour failed: Signature not found. (plugin needs updating)\n");
		}

		dlclose( lHandle );
	} else {
		LOG("AdditionalCollisionChecksThisTick: Detour failed. File changed?\n");
	}
	
	return 0;
}

extern "C" __attribute__( ( visibility("default") ) ) int gmod13_close( lua_State* LL )
{
	L = NULL;
	return 0;
}