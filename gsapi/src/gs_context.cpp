
#include "gs_context.h"

#include "netadr.h"

#define HANDLE_CALLRESULT( type, apiCall ) \
	case type##_t::k_iCallback: \
	{ \
		type##_t type##param; \
		bool type##bFailed; \
		\
		if ( !steamUtils->GetAPICallResult( apiCall, &type##param, sizeof( type##_t ), type##_t::k_iCallback, &type##bFailed ) ) \
		{ \
			Warning( "GSAPI: Unable to get API call result!\n" ); \
			return; \
		} \
		\
		if ( type##bFailed ) \
		{ \
			ESteamAPICallFailure callFailure = steamUtils->GetAPICallFailureReason( apiCall ); \
			Warning( "GSAPI: API call failed: %d\n", callFailure ); \
			return; \
		} \
		this->Handle##type( & type##param ); \
	} \
	break;


CGSContext::~CGSContext()
{
	apicontext.Clear();
}


void CGSContext::Init()
{
	if ( apicontext_ok ) return;
	apicontext_ok = apicontext.Init();
	
	
	if (!apicontext_ok) {
		Warning("[GSAPI] Failed to attach to server\n");
	}
}

bool CGSContext::IsLoggedOn()
{
	return apicontext.SteamGameServer()->BLoggedOn();
}

bool CGSContext::IsSecure()
{
	return apicontext.SteamGameServer()->BSecure();
}

CSteamID CGSContext::GetSteamID()
{
	return apicontext.SteamGameServer()->GetSteamID();
}

uint32 CGSContext::GetPublicIP()
{
	return apicontext.SteamGameServer()->GetPublicIP();
}

bool CGSContext::UpdateUserData( CSteamID steamID, const char *pchPlayerName, uint32 uScore )
{
	return apicontext.SteamGameServer()->BUpdateUserData( steamID, pchPlayerName, uScore );
}


CSteamID CGSContext::CreateUnauthenticatedUserConnection()
{
	return apicontext.SteamGameServer()->CreateUnauthenticatedUserConnection();
}

void CGSContext::SendUserDisconnect( CSteamID steamIDUser )
{
	apicontext.SteamGameServer()->SendUserDisconnect( steamIDUser );
}

void CGSContext::SetKeyValue( const char *key, const char *val )
{
	apicontext.SteamGameServer()->SetKeyValue( key, val );
}

void CGSContext::ClearAllKeyValues()
{
	apicontext.SteamGameServer()->ClearAllKeyValues( );
}

void CGSContext::SetGameTags( const char *pchGameTags )
{
	apicontext.SteamGameServer()->SetGameTags( pchGameTags );
}

// gameserverstats functions
bool CGSContext::RequestUserStats( CSteamID steamID )
{
	SteamAPICall_t apiCall = apicontext.SteamGameServerStats()->RequestUserStats( steamID );

	if ( apiCall == k_uAPICallInvalid )
		return false;

	CallResult_t callResult;
	callResult.m_iCallback = GSStatsReceived_t::k_iCallback;

	m_mapCallResults.Insert( apiCall, callResult );

	return true;
}

bool CGSContext::GetUserStat( CSteamID steamID, const char *pchName, int32 *pData )
{
	return apicontext.SteamGameServerStats()->GetUserStat( steamID, pchName, pData );
}

bool CGSContext::GetUserStat( CSteamID steamID, const char *pchName, float *pData )
{
	return apicontext.SteamGameServerStats()->GetUserStat( steamID, pchName, pData );
}

bool CGSContext::GetUserAchievement( CSteamID steamID, const char *pchName, bool *pbAchieved )
{
	return apicontext.SteamGameServerStats()->GetUserAchievement( steamID, pchName, pbAchieved );
}


/*
ILuaObject *CGSContext::TableFromCGameID( CGameID gameId )
{
	ILuaObject *gameTable = LUA->GetNewTable();

	gameTable->SetMember( "AppID", (float)gameId.AppID() );
	gameTable->SetMember( "ModID", (float)gameId.ModID() );
	gameTable->SetMember( "IsMod", gameId.IsMod() );
	gameTable->SetMember( "IsSteamApp", gameId.IsSteamApp() );
	gameTable->SetMember( "IsShortcut", gameId.IsShortcut() );
	gameTable->SetMember( "IsP2PFile", gameId.IsP2PFile() );
	gameTable->SetMember( "IsValid", gameId.IsValid() );

	return gameTable;
}

*/


#define PushSteamID(steamid) \
	{ \
		char strsid64 [50];                                    \
		strsid64[0]=0;                                         \
		sprintf(strsid64, "%llu", steamid.ConvertToUint64());  \
		LUA->PushString( strsid64 );                                    \
	}
	
void CGSContext::Steam_OnClientApprove(GSClientApprove_t *gsclient)
{
	NEED_LUA
	
	lhook("GSClientApprove");
		PushSteamID(gsclient->m_SteamID);
		PushSteamID(gsclient->m_OwnerSteamID);
		//LUA->PushString( gsclient->m_SteamID.Render() );
	lhook_call();
};

void CGSContext::Steam_OnClientDeny(GSClientDeny_t *gsclient)
{
	NEED_LUA
	
	lhook("GSClientDeny");
		PushSteamID(gsclient->m_SteamID);
		//LUA->PushString( gsclient->m_SteamID.Render() );
		LUA->PushNumber( (double)gsclient->m_eDenyReason );
		LUA->PushString( gsclient->m_rgchOptionalText );
	lhook_call();
};

void CGSContext::Steam_OnClientKick(GSClientKick_t *gsclient)
{
	NEED_LUA
	
	lhook("GSClientKick");
		PushSteamID(gsclient->m_SteamID);
		//LUA->PushString( gsclient->m_SteamID.Render() );
		LUA->PushNumber( (float)gsclient->m_eDenyReason );	
	lhook_call();

};

void CGSContext::Steam_OnValidateAuthTicketResponse(ValidateAuthTicketResponse_t *gsclient)
{
	NEED_LUA
	
	lhook("GSAuthResponse");
		PushSteamID(gsclient->m_SteamID);
		LUA->PushNumber( (float)gsclient->m_eAuthSessionResponse );
		PushSteamID(gsclient->m_OwnerSteamID);
	lhook_call();
	
};


void CGSContext::Steam_OnCallCompleted( SteamAPICallCompleted_t *pParam )
{
	if ( !apicontext_ok )
		return;
	
	SteamAPICall_t apiCall = pParam->m_hAsyncCall;

	ISteamUtils *steamUtils = apicontext.SteamGameServerUtils();

	if ( apiCall == k_uAPICallInvalid )
	{
		return;
	}

	CallResultIndex index = m_mapCallResults.Find( apiCall );

	if ( index == m_mapCallResults.InvalidIndex() )
	{
		return;
	}

	CallResult_t callResult = m_mapCallResults[ index ];

	m_mapCallResults.RemoveAt( index );

	switch ( callResult.m_iCallback )
	{
		HANDLE_CALLRESULT( GSStatsReceived, apiCall );
		HANDLE_CALLRESULT( GSStatsUnloaded, apiCall );
	}

}

void CGSContext::HandleGSStatsReceived( GSStatsReceived_t *pParam )
{
	NEED_LUA
	
	lhook("GSStatsReceived");

		LUA->PushNumber( (float)pParam->m_eResult );
		
		PushSteamID(pParam->m_steamIDUser);
		//LUA->PushString( pParam->m_steamIDUser.Render() );
	
	lhook_call();

}

void CGSContext::HandleGSStatsUnloaded( GSStatsUnloaded_t *pParam )
{
	NEED_LUA
	
	lhook("GSStatsUnloaded");
	
		PushSteamID(pParam->m_steamIDUser);
		//LUA->PushString( pParam->m_steamIDUser.Render() );
	
	lhook_call();

}


void CGSContext::Steam_OnStatsUnloaded( GSStatsUnloaded_t *pParam )
{
	NEED_LUA
	
	lhook("GSStatsUnloaded");

		PushSteamID(pParam->m_steamIDUser);
		//LUA->PushString( pParam->m_steamIDUser.Render() );

	lhook_call();
}


void CGSContext::Steam_OnPolicyResponse( GSPolicyResponse_t *pPolicyResponse )
{
	this->Init();
	
	NEED_LUA
	
	lhook("GSPolicyResponse");
		LUA->PushBool( (bool)pPolicyResponse->m_bSecure );
	lhook_call();
}


void CGSContext::Steam_OnConnect( SteamServersConnected_t *pParam )
{

	this->Init();
	
	NEED_LUA
	
	lhook("GSSteamConnected");
		LUA->PushNumber( true );
	lhook_call();
}

void CGSContext::Steam_OnDisconnect( SteamServersDisconnected_t *pParam )
{

	this->Init();
	
	NEED_LUA
	
	lhook("GSSteamDisconnected");
		LUA->PushNumber( (double) pParam->m_eResult );
	lhook_call();
}

void CGSContext::Steam_OnConnectFailure( SteamServerConnectFailure_t *pParam )
{
	this->Init();
	
	NEED_LUA
	
	lhook("GSSteamConnectFailure");
		LUA->PushNumber( (double) pParam->m_eResult );
	lhook_call();
	
}

// clientside, cba, doesn not concern servers
void CGSContext::Steam_OnClientGameServerDeny( ClientGameServerDeny_t *pParam )
{
	this->Init();
	
	NEED_LUA
	
	lhook("GSServerDeny");
		LUA->PushString( "TODO" );
	lhook_call();
}



bool CGSContext::RequestGroupStatus( CSteamID steamUser, CSteamID steamGroup )
{
	GSAPI()

	return apicontext.SteamGameServer()->RequestUserGroupStatus( steamUser, steamGroup );
}


void CGSContext::Steam_OnP2PSessionRequest( P2PSessionRequest_t *pCallback )
{
	NEED_LUA
	
	lhook("OnP2PSessionRequest");
	
		PushSteamID(pCallback->m_steamIDRemote);

	lhook_call();
	
}

void CGSContext::Steam_OnGSGroupStatus( GSClientGroupStatus_t *pParam )
{
	NEED_LUA
	
	lhook("GSGroupStatus");
	
		PushSteamID(pParam->m_SteamIDUser);
		PushSteamID(pParam->m_SteamIDGroup);
		//LUA->PushString( pParam->m_SteamIDUser.Render() );
		//LUA->PushString( pParam->m_SteamIDGroup.Render() );
		LUA->PushBool( pParam->m_bMember );
		LUA->PushBool( pParam->m_bOfficer );

	lhook_call();
}

CGSContext *g_pGSContext = NULL;




LUA_FUNCTION( ReqGroupStatus )
{

	GSAPI()

	LUA->CheckType( 2, GarrysMod::Lua::Type::STRING );

	const char *strSteamGroup = LUA->GetString( 2 );

	CSteamID steamID;
	SteamIDFromLua(1,steamID);

	if ( !( steamID.IsValid() && steamID.BIndividualAccount() ) )
	{
		LUA->ThrowError( "GSAPI: Invalid SteamID!" );
		return 0;
	}

	uint64 steamGrp64 = _atoi64( strSteamGroup );
	
	if ( steamGrp64==0 )
	{
		LUA->ThrowError( "GSAPI: Invalid Group ID!!" );
		return 0;
	}
	
	CSteamID steamGroup( steamGrp64 );
	
	if ( !( steamGroup.IsValid() ) )
	{
		LUA->ThrowError( "GSAPI: Invalid Group ID!" );
		return 0;
	}

	LUA->PushBool( g_pGSContext->RequestGroupStatus( steamID, steamGroup ) );
	return 1;
}


/*

CSteamID CGSContext::CreateUnauthenticatedUserConnection()
{
	if ( !apicontext_ok )
	{
		LUA->ThrowError( "GSAPI: GS context isn't initialized.\n" );
		return k_steamIDNil;
	}

	return apicontext.SteamGameServer()->CreateUnauthenticatedUserConnection();
}

void CGSContext::SendUserDisconnect( CSteamID steamIDUser )*/

LUA_FUNCTION( CreateUnauthenticatedUserConnection )
{
	CSteamID steamid = g_pGSContext->CreateUnauthenticatedUserConnection( );
	
	char strsid64 [50];
	strsid64[0]=0;
	sprintf(strsid64, "%llu", steamid.ConvertToUint64());
	
	LUA->PushString(strsid64);
	return 1;
}

LUA_FUNCTION( GetServerRealTime )
{
	GSAPI()
	
	ISteamUtils *steamUtils = g_pGSContext->apicontext.SteamGameServerUtils();

	uint32 steamtime = steamUtils->GetServerRealTime();
	
	LUA->PushNumber(steamtime);
	
	return 1;
	
}

LUA_FUNCTION( SendUserDisconnect )
{

	GSAPI()

	LUA->CheckType( 1, GarrysMod::Lua::Type::STRING );

	CSteamID steamID;
	SteamIDFromLua(1,steamID);
	
	if ( ! steamID.IsValid()  )
	{
		LUA->ThrowError( "GSAPI: Invalid SteamID!" );
		return 0;
	}

	g_pGSContext->SendUserDisconnect( steamID );
	return 0;
}


LUA_FUNCTION( IsLoggedOn )
{
	GSAPI()

	LUA->PushBool( g_pGSContext->IsLoggedOn() );
	return 1;
}

LUA_FUNCTION( IsSecure )
{
	GSAPI()

	LUA->PushBool( g_pGSContext->IsSecure() );
	return 1;
}

LUA_FUNCTION( GetSteamID )
{
	GSAPI()

	PushSteamID(g_pGSContext->GetSteamID());
	//LUA->PushString( g_pGSContext->GetSteamID().Render() );
	return 1;
}

LUA_FUNCTION( GetPublicIP )
{
	GSAPI()

	netadr_t netAddr( g_pGSContext->GetPublicIP(), 0 );

	LUA->PushString( netAddr.ToString( true ) );
	return 1;
}

LUA_FUNCTION( UpdateUserData )
{
	GSAPI()

	LUA->CheckType( 2, GarrysMod::Lua::Type::STRING );
	LUA->CheckType( 3, GarrysMod::Lua::Type::NUMBER );

	CSteamID steamID;
	SteamIDFromLua(1,steamID);
	
	if ( ! steamID.IsValid()  )
	{
		LUA->ThrowError( "GSAPI: Invalid SteamID!" );
		return 0;
	}

	const char *pchName = LUA->GetString( 2 );
	uint32 uScore = LUA->GetNumber( 3 );

	bool bRet = g_pGSContext->UpdateUserData( steamID, pchName, uScore );

	LUA->PushBool( bRet );
	return 1;
}

LUA_FUNCTION( ClearAllKeyValues )
{
	GSAPI()

	g_pGSContext->ClearAllKeyValues( );
	return 0;
}


LUA_FUNCTION( SetKeyValue )
{
	GSAPI()

	LUA->CheckType( 1, GarrysMod::Lua::Type::STRING );
	LUA->CheckType( 2, GarrysMod::Lua::Type::STRING );

	const char *key = LUA->GetString( 1 );
	const char *val = LUA->GetString( 2 );

	g_pGSContext->SetKeyValue( key, val );
	
	return 0;
}


LUA_FUNCTION( SetGameTags )
{
	GSAPI()

	LUA->CheckType( 1, GarrysMod::Lua::Type::STRING );

	const char *pchGameTags = LUA->GetString( 1 );

	g_pGSContext->SetGameTags( pchGameTags );
	return 0;
}



LUA_FUNCTION( RequestUserStats )
{
	GSAPI()
/*
	LUA->CheckType( 1, GarrysMod::Lua::Type::STRING );

	const char *pchSteamID = LUA->GetString( 1 );
	CSteamID steamID( pchSteamID, k_EUniversePublic  );

	if ( !( steamID.IsValid() && steamID.BIndividualAccount() ) )
	{
		LUA->ThrowError( "GSAPI: Invalid SteamID!" );
		return 0;
	}

	LUA->Push( g_pGSContext->RequestUserStats( steamID ) );
return 1;*/
}

LUA_FUNCTION( GetUserStatInt )
{
	GSAPI()
/*	
	LUA->CheckType( 1, GarrysMod::Lua::Type::STRING );
	LUA->CheckType( 2, GarrysMod::Lua::Type::STRING );

	const char *pchSteamID = LUA->GetString( 1 );
	CSteamID steamID( pchSteamID, k_EUniversePublic  );

	if ( !( steamID.IsValid() && steamID.BIndividualAccount() ) )
	{
		LUA->ThrowError( "GSAPI: Invalid SteamID!" );
		return 0;
	}

	const char *pchName = LUA->GetString( 2 );

	int32 iData = 0;
	bool bRet = g_pGSContext->GetUserStat( steamID, pchName, &iData );

	LUA->Push( bRet );

	if ( !bRet )
		return 1;

	LUA->Push( (float)iData );
	return 2;
*/
}

LUA_FUNCTION( GetUserStatFloat )
{
	GSAPI()
/*
	LUA->CheckType( 1, GarrysMod::Lua::Type::STRING );
	LUA->CheckType( 2, GarrysMod::Lua::Type::STRING );

	const char *pchSteamID = LUA->GetString( 1 );
	CSteamID steamID( pchSteamID, k_EUniversePublic  );

	if ( !( steamID.IsValid() && steamID.BIndividualAccount() ) )
	{
		LUA->ThrowError( "GSAPI: Invalid SteamID!" );
		return 0;
	}

	const char *pchName = LUA->GetString( 2 );

	float fData = 0.0f;
	bool bRet = g_pGSContext->GetUserStat( steamID, pchName, &fData );

	LUA->Push( bRet );

	if ( !bRet )
		return 1;

	LUA->Push( fData );
	return 2;
*/
}

LUA_FUNCTION( GetUserAchievement )
{
	GSAPI()
/*
	LUA->CheckType( 1, GarrysMod::Lua::Type::STRING );
	LUA->CheckType( 2, GarrysMod::Lua::Type::STRING );

	const char *pchSteamID = LUA->GetString( 1 );
	CSteamID steamID( pchSteamID, k_EUniversePublic  );

	if ( !( steamID.IsValid() && steamID.BIndividualAccount() ) )
	{
		LUA->ThrowError( "GSAPI: Invalid SteamID!" );
		return 0;
	}

	const char *pchName = LUA->GetString( 2 );

	bool bAchieved = false;
	bool bRet = g_pGSContext->GetUserAchievement( steamID, pchName, &bAchieved );

	LUA->Push( bRet );

	if ( !bRet )
		return 1;

	LUA->Push( bAchieved );
	return 2;
*/
}
