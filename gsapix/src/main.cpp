#include "main.h"

using namespace GarrysMod::Lua;


CBaseServer * pServer = NULL;
CSteam3Server * steamptr = NULL;
lua_State* L = NULL;


// RecalculateTags
typedef void (*tRecalculateTags) ();
tRecalculateTags original_RecalculateTags = NULL;
MologieDetours::Detour<tRecalculateTags>* detour_RecalculateTags = NULL;





void __cdecl hook_RecalculateTags ()
{	
}



GMOD_MODULE_OPEN()
{
	
	L=state;
	
	void *lHandle = dlopen( "engine_srv.so", RTLD_LAZY );
	if ( !lHandle ) return 0;

	pServer = (CBaseServer*)ResolveSymbol( lHandle, "sv" );
	if ( !pServer ) return 0;
	

	original_RecalculateTags = (tRecalculateTags)ResolveSymbol( lHandle, "_ZN11CBaseServer15RecalculateTagsEv" );
	if (original_RecalculateTags) {
		try {
			detour_RecalculateTags = new MologieDetours::Detour<tRecalculateTags>(original_RecalculateTags, hook_RecalculateTags);
		}
		catch(MologieDetours::DetourException &e) {
			Warning("== FAIL RecalculateTags ==\n");
		}
	}	
	

	dlclose( lHandle );
	
	
	return 0;
}

GMOD_MODULE_CLOSE()
{
	if (detour_RecalculateTags) 					delete detour_RecalculateTags;
	
	L=NULL;
	return 0;
}

