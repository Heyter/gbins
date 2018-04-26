

#include "gs_context.h"

#include "netadr.h"


void _SteamIDFromLua(unsigned int pos,CSteamID* steamID)
{
	lua_State* state = g_pGSContext->state;
	
	LUA->CheckType( pos, GarrysMod::Lua::Type::STRING );
	const char *pchSteamID = LUA->GetString( pos );
	if (!pchSteamID) return;
	
	uint64 steamid64 = _atoi64( pchSteamID );
	if (steamid64==0) return;
	
	steamID->SetFromUint64(steamid64);
	
	
}

int lua_lhook_toppos_start2 = 0;
int lua_lhook_toppos_start = 0;
void lhook(const char * name) 
{
	
	lua_State* state = g_pGSContext->state;
	
	lua_lhook_toppos_start = LUA->Top();
	
		LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB ); // Push ( 1 )
		LUA->GetField( -1, "hook" );                      // Push ( 1 )
		LUA->GetField( -1, "Run" );                       // Push ( 1 )
		
		LUA->PushString( name );   
	#ifdef DEBUG 
		Warning("HOOK: %s\n",name);
	#endif
	
	lua_lhook_toppos_start2 = LUA->Top();
	
};

void lhook_end()
{
	lua_State* state = g_pGSContext->state;
	
	int current_top = LUA->Top();
	int pop_count = current_top-lua_lhook_toppos_start;
	if (pop_count>0) {
		LUA->Pop( pop_count );
	}
	
};

bool lhook_call() {
	bool ret = lhook_call(0);
	lhook_end();
	return ret;
};

// legacy
bool lhook_call(unsigned int numargs,unsigned int rets) {
	bool ret = lhook_call(rets);
	return ret;
};

bool lhook_call(unsigned int rets) {
	
	lua_State* state = g_pGSContext->state;
	
	int top_pos = LUA->Top();
	int topcount = top_pos-lua_lhook_toppos_start2;
	
	if( LUA->PCall( 1+topcount, rets, 0 ) != 0 ) 
	{
		Warning( "[GSAPI Callback Errored] %s\n", LUA->GetString( ) ); 
		lhook_end();
		return false;
	}
	
	return true;
};




#define AddBindingFunc( name, func )		\
	LUA->PushCFunction( func );	\
	LUA->SetField( -2, name );
	
GMOD_MODULE_OPEN( )
{
	if ( !g_pGSContext ) {
		g_pGSContext = new CGSContext( state );				
		g_pGSContext->Init();
	}
	
	g_pGSContext->state=state;	


	LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB );
	LUA->CreateTable( );
	
		AddBindingFunc( "IsLoggedOn", IsLoggedOn );
		AddBindingFunc( "IsSecure", IsSecure );
		AddBindingFunc( "GetSteamID", GetSteamID );
		AddBindingFunc( "GetPublicIP", GetPublicIP );
		AddBindingFunc( "CreateUnauthenticatedUserConnection", CreateUnauthenticatedUserConnection );
		AddBindingFunc( "GetServerRealTime", GetServerRealTime );
		AddBindingFunc( "SendUserDisconnect", SendUserDisconnect );
		AddBindingFunc( "UpdateUserData", UpdateUserData );
		
		AddBindingFunc( "SetGameTags", SetGameTags );
		AddBindingFunc( "SetMapName", SetMapName );
		AddBindingFunc( "SetKeyValue", SetKeyValue );
		AddBindingFunc( "ClearAllKeyValues", ClearAllKeyValues );
		
		AddBindingFunc( "RequestGroupStatus", ReqGroupStatus );
		
	LUA->SetField( -2, "gameserver" );

	
	LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB );
	LUA->CreateTable( );

		AddBindingFunc( "RequestUserStats", RequestUserStats );
		AddBindingFunc( "GetUserStatInt", GetUserStatInt );
		AddBindingFunc( "GetUserStatFloat", GetUserStatFloat );
		AddBindingFunc( "GetUserAchievement", GetUserAchievement );
		
	LUA->SetField( -2, "gameserverstats" );
	

	LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB );
	LUA->CreateTable( );
	
		AddBindingFunc( "RequestGroupStatus", ReqGroupStatus );
		
	LUA->SetField( -2, "furryfinder" );

	return 0;
}


GMOD_MODULE_CLOSE( )
{
	if (!g_pGSContext) return 0;
	
	g_pGSContext->state=NULL;
	return 0;
}
