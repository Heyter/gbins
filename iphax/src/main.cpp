#include <stdio.h>
#include "tier1.h"
#include "filesystem.h"
#include "engine/iserverplugin.h"
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
#include "interface.h"
#include "game/server/iplayerinfo.h"
#include <vstdlib/cvar.h>

#include "tier0/memdbgon.h"


IGameEventManager2 *gameeventmanager = NULL; 
inline bool FStrEq(const char *sz1, const char *sz2)
{
	return(Q_stricmp(sz1, sz2) == 0);
}

class Plugin: public IServerPluginCallbacks, public IGameEventListener2
{
public:
	Plugin();
	~Plugin();

	
	virtual bool			Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory );
	virtual void			Unload( void );
	virtual void			Pause( void );
	virtual void			UnPause( void );
	virtual const char     	*GetPluginDescription( void );      
	virtual void			LevelInit( char const *pMapName );
	virtual void			ServerActivate( edict_t *pEdictList, int edictCount, int clientMax );
	virtual void			GameFrame( bool simulating );
	virtual void			LevelShutdown( void );
	virtual void			ClientActive( edict_t *pEntity );
	virtual void			ClientDisconnect( edict_t *pEntity );
	virtual void			ClientPutInServer( edict_t *pEntity, char const *playername );
	virtual void			SetCommandClient( int index );
	virtual void			ClientSettingsChanged( edict_t *pEdict );
	virtual PLUGIN_RESULT	ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen );
	virtual PLUGIN_RESULT	ClientCommand( edict_t *pEntity, const CCommand &args );
	virtual PLUGIN_RESULT	NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );
	virtual void			OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue );

	
	virtual void			OnEdictAllocated( edict_t *edict );
	virtual void			OnEdictFreed( const edict_t *edict  );	

	
	virtual void FireGameEvent(  IGameEvent * event);

private:
	virtual void listen();	
	virtual void unlisten();
};

Plugin g_EmtpyServerPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(Plugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_EmtpyServerPlugin );
Plugin::Plugin(){}
Plugin::~Plugin(){}
bool loaded = false;
void Plugin::listen()
{ 
	if (!loaded) {
		gameeventmanager->AddListener(this, "player_connect", true /* server side? */ );
		loaded = true;
	}
}
void Plugin::unlisten()
{ 
	if (loaded) {
		gameeventmanager->RemoveListener( this ); 
		loaded = false;
	}
}

bool Plugin::Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{
	CreateInterfaceFn tier1Factory = VStdLib_GetICVarFactory();
	g_pCVar = (ICvar *)tier1Factory( CVAR_INTERFACE_VERSION, NULL );
	ConVar_Register( 0 );
	
	gameeventmanager = (IGameEventManager2 *)interfaceFactory(INTERFACEVERSION_GAMEEVENTSMANAGER2,NULL);
	
	if(	! ( gameeventmanager ) ) {
		Warning("player_connect address changer FAILED LOADING.\n");
		return false; 
	}
	else
		listen();
	Msg("player_connect address changer loaded.\n");


	return true;
}

void Plugin::Unload( void )
{
	unlisten();
	ConVar_Unregister( );
	//DisconnectTier1Libraries( );
}

const char *Plugin::GetPluginDescription( void )
{
	return "iphax, by Python1320";
}

void Plugin::LevelInit( char const *pMapName ){
 listen(); 
}
void Plugin::LevelShutdown( void ) 
{
	unlisten();
}

static ConVar newip("net_event_playerip", "0.0.0.0:27005", FCVAR_ARCHIVE, "Set the ip to be announced for joining players");

void Plugin::FireGameEvent(  IGameEvent * event)
{
	const char * name = event->GetName();
	 if( FStrEq(name, "player_connect") ) {
	//	Msg("player_connect address: '%s' -> '%s'\n",event->GetString( "address" ) , newip.GetString() );
		event->SetString( "address", newip.GetString() );
	}
}




// i uh... nope
void Plugin::Pause( void ){}
void Plugin::UnPause( void ){}
void Plugin::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax ){}
void Plugin::GameFrame( bool simulating ){}
void Plugin::ClientActive( edict_t *pEntity ){}
void Plugin::ClientDisconnect( edict_t *pEntity ){}
void Plugin::ClientPutInServer( edict_t *pEntity, char const *playername ){}
void Plugin::SetCommandClient( int index ){}
void Plugin::ClientSettingsChanged( edict_t *pEdict ){}
PLUGIN_RESULT Plugin::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen ){	return PLUGIN_CONTINUE; }
PLUGIN_RESULT Plugin::ClientCommand( edict_t *pEntity, const CCommand &args ) {	return PLUGIN_CONTINUE; }
PLUGIN_RESULT Plugin::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID ) {	return PLUGIN_CONTINUE; }
void Plugin::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue ) {}
void Plugin::OnEdictAllocated( edict_t *edict ){}
void Plugin::OnEdictFreed( const edict_t *edict  ){}
