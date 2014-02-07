
#define VTABLE_OFFSET 1

#include <dlfcn.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>

#include "sourcemod/memutils.h"

#include "vfnhook.h"

#include <tier1/tier1.h>
#include <tier1/convar.h>

#include <interface.h>
#include <netadr.h>
#include <inetmsghandler.h>
#include <inetchannel.h>
#include <steamclientpublic.h>
#include <vstdlib/cvar.h>
#include <dbg.h>
#undef min
#undef max
#include "detours.h"

class CBaseClient;
class CSteam3Server;

#include "ILuaModuleManager.h"
//#include "common/GMLuaModule.h"

GMOD_MODULE( Load, Unload );


//ILuaInterface* gLua = NULL;
lua_State* LL = NULL;
typedef const char * (*tGetNetworkIDString)(CBaseClient * );
tGetNetworkIDString CBaseClient__GetNetworkIDString = NULL;

typedef int (*tGetUserID)(CBaseClient * );
tGetUserID CBaseClient__GetUserID = NULL;

typedef bool (*tExecuteStringCommand) (CBaseClient * , const char * );
tExecuteStringCommand original_ExecuteStringCommand = NULL;
MologieDetours::Detour<tExecuteStringCommand>* detour_ExecuteStringCommand = NULL;

bool __cdecl hook_ExecuteStringCommand (CBaseClient * client, const char *pCommand)
{	
	bool result = detour_ExecuteStringCommand->GetOriginalFunction() ( client, pCommand );
	
	lua_State* L = LL;
	
	int uid = -1;
	if (CBaseClient__GetUserID) uid = CBaseClient__GetUserID( client );
	
	ILuaObject *hookTable = Lua()->Global()->GetMember( "hook" );
		ILuaObject *hookCallFunc = hookTable->GetMember( "Call" );
		
			Lua()->Push( hookCallFunc );
				Lua()->Push( "ExecuteStringCommand" );
				Lua()->PushNil();
				
				if ( !pCommand || !pCommand[0] )
					Lua()->PushNil();
				else
					Lua()->Push( pCommand );
					
				
				Lua()->Push( uid );	
				Lua()->Push( CBaseClient__GetNetworkIDString?CBaseClient__GetNetworkIDString( client ):"wtf" );				
			Lua()->Call( 5, 1 );

		hookTable->UnReference();
	hookCallFunc->UnReference();

	ILuaObject *ret = Lua()->GetReturn(0);

		if ( ret->GetType() == Type::BOOL )
		{   
			result = ret->GetBool();
		}
		
	ret->UnReference();

	return result;
}

int Load( lua_State *L )
{
	void *lHandle = dlopen( "engine_srv.so", RTLD_LAZY );
	if ( lHandle )
	{
		CBaseClient__GetUserID = ResolveSymbol( lHandle, "_ZNK11CBaseClient9GetUserIDEv" );
		CBaseClient__GetNetworkIDString = ResolveSymbol( lHandle, "_ZNK11CBaseClient18GetNetworkIDStringEv" );
		original_ExecuteStringCommand = ResolveSymbol( lHandle, "_ZN11CBaseClient20ExecuteStringCommandEPKc" );
		if (original_ExecuteStringCommand) {
			try {
				detour_ExecuteStringCommand = new MologieDetours::Detour<tExecuteStringCommand>(original_ExecuteStringCommand, hook_ExecuteStringCommand);
			}
			catch(MologieDetours::DetourException &e) {
			}
			dlclose( lHandle );
		} else Warning("gmsv_slog: Signature failed\n");
	
	}
	LL=L;
	return 0;
}

int Unload( lua_State *L )
{
	delete detour_ExecuteStringCommand;
	return 0;
}
