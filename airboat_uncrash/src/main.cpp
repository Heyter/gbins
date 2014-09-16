
#include <dlfcn.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "memutils.h"
#include "detours.h"
#include "tier0/dbg.h"

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
}


class CBaseEntity;
class Vector;
enum ShakeCommand_t
{
        SHAKE_START = 0,       
        SHAKE_STOP,            
        SHAKE_AMPLITUDE,       
        SHAKE_FREQUENCY,       
        SHAKE_START_RUMBLEONLY,
        SHAKE_START_NORUMBLE,  
};


typedef void (*	tScreenShakeObject ) ( CBaseEntity *, const Vector &, float , float , float , float , ShakeCommand_t , bool ) ;
tScreenShakeObject original_ScreenShakeObject = NULL;

lua_State* L = NULL;

MologieDetours::Detour<tScreenShakeObject>* detour_ScreenShakeObject = NULL;


void hook_ScreenShakeObject( CBaseEntity *pEnt, const Vector &center, float amplitude, float frequency, float duration, float radius, ShakeCommand_t eCommand, bool bAirShake )
{
	static int q = 10;
	q++;
	if (q>10) {
		q = 0;
		Warning("+");
	}
	return;
}





extern "C" __attribute__( ( visibility("default") ) ) 
int gmod13_open( lua_State* LL )
{
	void *lHandle = dlopen( "garrysmod/bin/server_srv.so", RTLD_LAZY  );

	if ( !lHandle ) {
		Warning("handle failed???\n");
		return 0;
	}
	
	original_ScreenShakeObject = (tScreenShakeObject)ResolveSymbol( lHandle, "_Z22UTIL_ScreenShakeObjectP11CBaseEntityRK6Vectorffff14ShakeCommand_tb" );
	if (!original_ScreenShakeObject) {
		Warning("ScreenShakeObject: Detour failed: Signature not found. (plugin needs updating)\n");
		return 0;
	}

	try 
	{
		detour_ScreenShakeObject = new MologieDetours::Detour<tScreenShakeObject>(original_ScreenShakeObject, hook_ScreenShakeObject);
	}
	catch(MologieDetours::DetourException &e) 
	{
		Warning("ScreenShakeObject: Detour failed: Internal error?\n");
		return 0;
	}
	
	return 0;
}

extern "C" __attribute__( ( visibility("default") ) ) 
int gmod13_close( lua_State* LL )
{
	delete detour_ScreenShakeObject;
	L = NULL;
	return 0;
}