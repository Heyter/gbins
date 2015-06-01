
#ifndef GSCONTEXT_H_
#define GSCONTEXT_H_

#ifdef _WIN32
#pragma once
#endif

#undef min
#undef max

#include "strtools.h"


#define LUA_FUNCTION( _function_ ) int _function_( lua_State* state )

#include <stdlib.h>
#ifndef _WIN32
        #define _atoi64(S)  atoll(S)
#endif


#define GSAPI() \
	if ( !g_pSteamClientGameServer || !g_pGSContext || !g_pGSContext->apicontext_ok ) \
	{ \
		LUA->ThrowError( "GsAPI: Not available!" ); \
		return 0; \
	}


#define NEED_LUA \
	if ( !g_pGSContext->state ) \
	{ \
		Warning( "GsAPI: Lua state missing!\n" ); \
		return; \
	};



#include "eiface.h"
#include "tier2/tier2.h"


#include "tier1/utlvector.h"
#include "tier1/utlmap.h"

#define DBGFLAG_ASSERT
#include "dbg.h"
	
// steamworks api
#include "steam/steam_api.h"
#include "steam_gameserver_modified.h"

#include <GarrysMod/Lua/Interface.h>

struct CallResult_t
{
	uint32 m_iCallback;
};

typedef CUtlMap<SteamAPICall_t, CallResult_t> CallResultMap;
typedef CallResultMap::IndexType_t CallResultIndex;


class CGSContext
{

public:
	CGSContext( lua_State *luaState ) :
		m_CallbackConnected( this, &CGSContext::Steam_OnConnect ),
		m_CallbackDisconnected( this, &CGSContext::Steam_OnDisconnect ),
		m_CallbackPolicyResponse( this, &CGSContext::Steam_OnPolicyResponse ),
		m_CallbackConnectFailure( this, &CGSContext::Steam_OnConnectFailure ),
		m_CallbackClientGameServerDeny( this, &CGSContext::Steam_OnClientGameServerDeny ),

		m_CallbackClientDeny( this, &CGSContext::Steam_OnClientDeny ),
		m_CallbackClientApprove( this, &CGSContext::Steam_OnClientApprove ),
		m_CallbackClientKick( this, &CGSContext::Steam_OnClientKick ),
		m_CallbackValidateAuthTicket( this, &CGSContext::Steam_OnValidateAuthTicketResponse ),
		
		m_CallbackGroupStatus( this, &CGSContext::Steam_OnGSGroupStatus ),
	
		m_CallbackCallCompleted( this, &CGSContext::Steam_OnCallCompleted ),
		m_CallbackStatsUnloaded( this, &CGSContext::Steam_OnStatsUnloaded ),

		state( luaState ),
		first_init( false ),
		apicontext_ok( false ),
		
		m_mapCallResults( DefLessFunc( uint64 ) ) 
	{
		// constructor			
	};

	~CGSContext();


	void Init();


	// gameserver functions
	bool IsLoggedOn();
	bool IsSecure();

	CSteamID GetSteamID();
	uint32 GetPublicIP();

	bool UpdateUserData( CSteamID steamID, const char *pchPlayerName, uint32 uScore );
	void SendUserDisconnect( CSteamID steamIDUser );
	CSteamID CreateUnauthenticatedUserConnection();

	bool SetServerType( uint32 unServerFlags, uint32 unGameIP, uint16 unGamePort, uint16 unSpectatorPort,
						uint16 usQueryPort, const char *pchGameDir,  const char *pchVersion, bool bLANMode );

	void SetGameTags( const char *pchGameTags );

	void GetGameStats();


	// gameserverstats functions
	bool RequestUserStats( CSteamID steamID );

	bool GetUserStat( CSteamID steamID, const char *pchName, int32 *pData );
	bool GetUserStat( CSteamID steamID, const char *pchName, float *pData );
	bool GetUserAchievement( CSteamID steamID, const char *pchName, bool *pbAchieved );

	bool RequestGroupStatus(CSteamID, CSteamID);

//private:
//	ILuaObject *TableFromCGameID( CGameID gameId );

private:
	// callbacks
	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnConnect, SteamServersConnected_t, m_CallbackConnected );
	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnDisconnect, SteamServersDisconnected_t, m_CallbackDisconnected );
	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnPolicyResponse, GSPolicyResponse_t, m_CallbackPolicyResponse );
	
	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnConnectFailure, SteamServerConnectFailure_t, m_CallbackConnectFailure );
	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnClientGameServerDeny, ClientGameServerDeny_t, m_CallbackClientGameServerDeny );

	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnClientDeny, GSClientDeny_t, m_CallbackClientDeny );
	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnClientApprove, GSClientApprove_t, m_CallbackClientApprove );
	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnClientKick, GSClientKick_t, m_CallbackClientKick );
	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnValidateAuthTicketResponse, ValidateAuthTicketResponse_t, m_CallbackValidateAuthTicket );

	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnGSGroupStatus, GSClientGroupStatus_t, m_CallbackGroupStatus );
	
	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnCallCompleted, SteamAPICallCompleted_t, m_CallbackCallCompleted );
	STEAM_GAMESERVER_CALLBACK( CGSContext, Steam_OnStatsUnloaded, GSStatsUnloaded_t, m_CallbackStatsUnloaded );

	// for api calls
	CallResultMap m_mapCallResults;

private:
	void HandleGSStatsReceived( GSStatsReceived_t *pParam );
	void HandleGSStatsUnloaded( GSStatsUnloaded_t *pParam );


public:
	lua_State *state;

	CSteamGameServerAPIContext apicontext;
	bool apicontext_ok;
	bool first_init;
};

extern CGSContext *g_pGSContext;

// gameserver funcs
LUA_FUNCTION( IsLoggedOn );
LUA_FUNCTION( IsSecure );
LUA_FUNCTION( GetSteamID );
LUA_FUNCTION( GetPublicIP );
LUA_FUNCTION( UpdateUserData );
LUA_FUNCTION( CreateUnauthenticatedUserConnection );
LUA_FUNCTION( GetServerRealTime );
LUA_FUNCTION( SendUserDisconnect );
LUA_FUNCTION( SetServerType );
LUA_FUNCTION( SetGameTags );
LUA_FUNCTION( GetGameStats );

// gameserverstats funcs
LUA_FUNCTION( RequestUserStats );
LUA_FUNCTION( GetUserStatInt );
LUA_FUNCTION( GetUserStatFloat );
LUA_FUNCTION( GetUserAchievement );
LUA_FUNCTION( ReqGroupStatus );

// silly helper functions
void lhook(const char * name);
bool lhook_call(unsigned int numargs,unsigned int rets);
bool lhook_call(unsigned int rets);
bool lhook_call();
void lhook_end();


#define SteamIDFromLua(pos,steamID) _SteamIDFromLua(pos,&steamID)
void _SteamIDFromLua(unsigned int pos,CSteamID* steamID);


#endif


