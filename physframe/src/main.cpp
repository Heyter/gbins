
#include <string.h>
#include <stdio.h>

#include <dlfcn.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "memutils.h"

// #define GAME_DLL 1
// #include "cbase.h"
// #include "utlvector.h"
// #include "igamesystem.h"
// #include "vphysics_interface.h"
// #include "physics_shared.h"
// #include "iservervehicle.h"
// #include "vehicle_sounds.h"
// #include "vphysics_sound.h"

//#include "vfnhook.h"

#undef min
#undef max
#include "detours.h"

#include "tier0/dbg.h"


extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
}



// SEE IF I CARE ANYMORE
class CPhysicsHook;
typedef CPhysicsHook* (*tPhysicsGameSystem ) ( ) ;
tPhysicsGameSystem func_PhysicsGameSystem = NULL;
void SetPhysPaused(bool should) 
{
	int physhook_class = (int)func_PhysicsGameSystem();
	bool *m_bPaused;
	m_bPaused = (bool *)(physhook_class + 88); // from CPhysicsHook::LevelInitPostEntity() 
	*m_bPaused = should;
}





lua_State* L = NULL;

typedef void		 (*	tPhysFrame ) 						( float ) ;
						tPhysFrame 		original_PhysFrame	= NULL;
MologieDetours::Detour< tPhysFrame>*	detour_PhysFrame	= NULL;


void hook_PhysFrame( float deltaTime )
{
	if (true) {
		return detour_PhysFrame->GetOriginalFunction()( deltaTime );
	}
	
	if (!L) {
		return detour_PhysFrame->GetOriginalFunction()( deltaTime );
	}
	
	lua_getglobal(L, "PhysFrame");
	
	if (lua_isnil(L,-1)) {
		lua_pop(L, 1);
		return detour_PhysFrame->GetOriginalFunction()( deltaTime );
	}
	
	if (lua_pcall(L, 0, 1, 0) == 0) {
		
		if (lua_isboolean(L, -1)) {
			bool ret = lua_toboolean(L, -1);
			
			// block physframe
			if (ret) {
				lua_pop(L, 1);
				return;
			}
		}
		
		lua_pop(L, 1); // pop result
		
	} else { // errored
		const char* err = lua_tostring(L, -1);
		Warning("PhysFrame: %s\n",err);
		lua_pop(L, 1); // pop error
		
	}
	
	return detour_PhysFrame->GetOriginalFunction()( deltaTime );
}


//class IVP_OV_Node;
typedef void (*	texpand_tree ) ( int tree, int howmuch ) ;
texpand_tree original_expand_tree = NULL;
MologieDetours::Detour<texpand_tree>* detour_expand_tree = NULL;


typedef unsigned long _DWORD;

void hook_expand_tree_fail() 
{
	static bool triggered = false;
	if (!L || triggered) return;
	triggered = true;
	
	lua_getglobal(L, "PhysFailed");
	
	if (lua_isnil(L,-1)) {
		Warning("*** ERRORHAX *** Excessive sizelevel?!!?\n");
		return;
	}
	
	if (lua_pcall(L, 0, 0, 0) == 0) {
		
	} else { // errored
		const char* err = lua_tostring(L, -1);
		Warning("PhysFailed: Hook failed: %s\n",err);
		lua_pop(L, 1); // error
	}
}

void hook_expand_tree( int tree, int howmuch )
{
	int treeparam1 = *(_DWORD *)(tree + 52);
	if ( *(_DWORD *)(treeparam1 + 16) > 40 )
	{
		hook_expand_tree_fail();
	}

	return detour_expand_tree->GetOriginalFunction()( tree, howmuch );
}

	
	
int SetShouldSimulate( lua_State* L ) 
{
	bool should = lua_toboolean(L,1);
	lua_pop(L, 1);
	
	SetPhysPaused(!should);
	
	return 0;
}

extern "C" __attribute__( ( visibility("default") ) ) int gmod13_open( lua_State* LL )
{
	L=LL;


	void *lHandle = dlopen( "garrysmod/bin/server_srv.so", RTLD_LAZY );
	if ( lHandle )
	{
		
		func_PhysicsGameSystem = (tPhysicsGameSystem)ResolveSymbol( lHandle, "_Z17PhysicsGameSystemv" );
		if (func_PhysicsGameSystem) {
			lua_pushcfunction(L,SetShouldSimulate);
			lua_setglobal(L, "SetShouldSimulate");
		} else {
			Warning("Function PhysicsGameSystem missing!!!\n");
		}
		dlclose( lHandle );
	} else 
	{
		Warning("Finding server_srv failed???\n");
	}
	
	
	void *lHandle2 = dlopen( "bin/vphysics_srv.so", RTLD_LAZY );

	if ( lHandle2 )
	{
		original_expand_tree = (texpand_tree)ResolveSymbol( lHandle2, "_ZN19IVP_OV_Tree_Manager11expand_treeEPK11IVP_OV_Node" );
		if (original_expand_tree) {
			try {
				detour_expand_tree = new MologieDetours::Detour<texpand_tree>(original_expand_tree, hook_expand_tree);
			}
			catch(MologieDetours::DetourException &e) {
				Warning("expand_tree: Detour failed: Internal error?\n");
			}
		} else {
			Warning("expand_tree: Detour failed: Signature not found. (plugin needs updating)\n");
		}

		dlclose( lHandle2 );
	}
	else 
	{
		Warning("handle failed???\n");
	}
	
	
	
	return 0;
}

extern "C" __attribute__( ( visibility("default") ) ) int gmod13_close( lua_State* LL )
{
	if (detour_PhysFrame) {
		delete detour_PhysFrame;
	}
	
	if (detour_expand_tree) {
		delete detour_expand_tree;
	}
	
	L = NULL;
	return 0;
}