

#include "memutils.h"

//#include "vfnhook.h"


#include "detours.h"

//#include "tier0/dbg.h"


extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
}


typedef bool (*	tIsValidFileForTransfer ) ( const char * ) ;
tIsValidFileForTransfer original_IsValidFileForTransfer = NULL;

lua_State* L = NULL;

MologieDetours::Detour<tIsValidFileForTransfer>* detour_IsValidFileForTransfer = NULL;


bool hook_IsValidFileForTransfer( const char * filepath )
{
	bool ret = false;

	ret = detour_IsValidFileForTransfer->GetOriginalFunction(filepath);
	
	if (!L) return ret;
	
	lua_getglobal(L, "hook");                 		
		lua_getfield(L, -1, "Run");              	
		lua_pushstring(L,"IsValidFileForTransfer");  	
		lua_pushstring(L, filepath);    	

		// do the call (2 arguments, 1 result) 		
		if (lua_pcall(L, 2, 1, 0) == 0) {
			
			if (lua_isboolean(L, -1)) {
				bool ret2 = lua_toboolean(L, -1);
				ret = ret2;
			}
			
			lua_pop(L, 1); // pop result
			
		} else { // errored
			const char* err = lua_tostring(L, -1);
			Warning("IsValidFileForTransfer: %s\n",err);
			lua_pop(L, 1); // pop error
			
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
		original_IsValidFileForTransfer = ResolveSymbol( lHandle, "_ZN8CNetChan22IsValidFileForTransferEPKc" );
		if (original_IsValidFileForTransfer) {
			try {
				detour_IsValidFileForTransfer = new MologieDetours::Detour<tIsValidFileForTransfer>(original_IsValidFileForTransfer, hook_IsValidFileForTransfer);
				loaded = true;
			}
			catch(MologieDetours::DetourException &e) {
				LOG("IsValidFileForTransfer: Detour failed: Internal error?\n");
			}
		} else {
			LOG("IsValidFileForTransfer: Detour failed: Signature not found. (plugin needs updating)\n");
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
	delete detour_IsValidFileForTransfer;
	L = NULL;
	return 0;
}