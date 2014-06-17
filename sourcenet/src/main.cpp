// Required interfaces
#define IVENGINESERVER_INTERFACE
#define IVENGINECLIENT_INTERFACE
#define ICVAR_INTERFACE

// Main header
#include "main.h"

// Engine headers
#include "net.h"
#include "protocol.h"

// Lua extensions
#include "gl_hooks.h"
#include "gl_bitbuf_write.h"
#include "gl_bitbuf_read.h"
#include "gl_cnetchan.h"
#include "gl_inetchannelhandler.h"
#include "gl_subchannel_t.h"
#include "gl_datafragments_t.h"
#include "gl_filehandle_t.h"
#include "gl_ucharptr.h"
#include "gl_netadr_t.h"
#include "gl_inetworkstringtablecontainer.h"
#include "gl_inetworkstringtable.h"
#include "gl_igameeventmanager2.h"
#include "gl_igameevent.h"

// Platform definitions

#ifdef _WIN32

#define ENGINE_LIB "engine.dll"
#define CLIENT_LIB "client.dll"
#define SERVER_LIB "server.dll"



#define CNetChan_ProcessMessages_SIG "\x55\x8B\xEC\x83\xEC\x2C\x53\x89\x4D\xFC"
#define CNetChan_ProcessMessages_MSK "xxxxxxxxxx"

#define NETPATCH_LEN 6
#define NETPATCH_OLD "\x0F\x84\xC1\x00\x00\x00"
#define NETPATCH_NEW "\xE9\x2B\x00\x00\x00\x90"
#define NETCHUNK_SIG_OFFSET 33
#define NETCHUNK_SIG "\x55\x8b\xec\xb8\x00\x00\x00\x00\xe8\x00\x00\x00\x00\xa1\x00\x00\x00\x00\x83\x78\x30\x00\x53\x8b\x5d\x08\x56\x8b\xf1\x57\x89\x75\xfc\x74\x00\x85\xdb\x74\x00\x8b\x43\x0c\x83\xc0\x07\xc1\xf8\x03\x85\xc0\x7e\x00\x8b\x0d\x00\x00\x00\x00\x8b\x11\x8b\x8a\x90\x00\x00\x00\x48\x50\x8b\x03\x50\x68\x00\x00\x00\x00‏"
#define NETCHUNK_MSK "xxxx????x????x????xxxxxxxxxxxxxxxx?xxx?xxxxxxxxxxxx?xx????xxxxxxxxxxxxxx????"

#elif defined _LINUX

#include "memutils.h"

#define ENGINE_LIB "engine_srv.so"
#define CLIENT_LIB NULL
#define SERVER_LIB "server_srv.so"

#define NETPATCH_LEN 6
#define NETPATCH_OLD "\x0F\x85\xC9\x00\x00\x00"
#define NETPATCH_NEW "\xE9\x01\x00\x00\x00\90"
#define NETCHUNK_SIG_OFFSET 8
#define NETCHUNK_SIG "\x85\xFF\x8D\x04\x91\x89\x46\x10"
#define NETCHUNK_MSK "xxxxxxxx"

#endif

// Entry points
GMOD_MODULE( Open, Close );

// Enable/disable SendDatagram hooking
bool g_bPatchedNetChunk;

// Multiple Lua state support
luaStateList_t g_LuaStates;
luaStateList_t *GetLuaStates( void ) { return &g_LuaStates; }

// Interfaces
CreateInterfaceFn fnEngineFactory = NULL;

IVEngineServer *g_pEngineServer = NULL;
IVEngineClient *g_pEngineClient = NULL;

ICvar *g_pCVarClient = NULL;
ICvar *g_pCVarServer = NULL;

// CSigScan wrapper
#include "../simplescan/csimplescan.h"

// CNetChan::ProcessMessages function pointer
void *CNetChan_ProcessMessages_T = NULL;

// Garbage collection
LUA_FUNCTION( std__gc )
{
	UsesLua();

	delete Lua()->GetUserData( 1 );

	return 0;
}

// Checks if the server is dedicated
GLBL_FUNCTION( sourcenet_isDedicatedServer )
{
	UsesLua();

	Lua()->Push( g_pEngineServer->IsDedicatedServer() );

	return 1;
}

// Module load
int Open( lua_State *L )
{
	UsesLua();

	// State descriptor
	multiStateInfo msi;
	msi.L = L;
	// hook.Call object
	ILuaObject *_G_hook = Lua()->GetGlobal( "hook" );
	ILuaObject *_G_hook_Call = _G_hook->GetMember( "Call" );
	_G_hook_Call->Push();
	// hook.Call reference
	//msi.ref_hook_Call = Lua()->GetReference( -1, true );
	msi.ref_hook_Call = Lua()->GetReference( -1 );
	// Cleanup
	_G_hook_Call->UnReference();
	_G_hook->UnReference();
	// Register state
	g_LuaStates.AddToTail( msi );
	
	fnEngineFactory = Sys_GetFactory( ENGINE_LIB );

	if ( !fnEngineFactory )
	{
		Lua()->Error( "Failed to retrieve engine factory function" );

		return 0;
	}

	g_pEngineServer = (IVEngineServer *)fnEngineFactory( INTERFACEVERSION_VENGINESERVER, NULL );

	if ( !g_pEngineServer )
	{
		Lua()->Error( "Failed to retrieve server engine interface" );

		return 0;
	}

	if ( Lua()->IsClient() || !g_pEngineServer->IsDedicatedServer() )
	{
		g_pEngineClient = (IVEngineClient *)fnEngineFactory( VENGINE_CLIENT_INTERFACE_VERSION, NULL );

		if ( !g_pEngineClient )
		{
			Lua()->Error( "Failed to retrieve client engine interface" );

			return 0;
		}

#ifdef _WIN32
		g_pCVarClient = *(ICvar **)GetProcAddress( GetModuleHandle( CLIENT_LIB ), "cvar" );
#endif

		if ( !g_pCVarClient )
		{
			Lua()->Error( "Failed to retrieve client cvar interface" );

			return 0;
		}
	}
	
	if ( Lua()->IsServer() || !g_pEngineServer->IsDedicatedServer() )
	{
#ifdef _WIN32
		g_pCVarServer = *(ICvar **)GetProcAddress( GetModuleHandle( SERVER_LIB ), "cvar" );
#elif defined _LINUX
		/*void *hServer = dlopen( SERVER_LIB, RTLD_NOW );

		if ( hServer )
		{
			g_pCVarServer = *(ICvar **)ResolveSymbol( hServer, "g_pCVar" );

			dlclose( hServer );

		}*/

		void *hEngine = dlopen( ENGINE_LIB, RTLD_NOW );

		if ( hEngine )
		{
			g_pCVarServer = *(ICvar **)ResolveSymbol( hEngine, "g_pCVar" );

			dlclose( hEngine );
		}
#endif

		if ( !g_pCVarServer )
		{
			Lua()->Error( "Failed to retrieve server cvar interface" );

			return 0;
		}
	}

	BEGIN_ENUM_REGISTRATION( UpdateType );
		REG_ENUM( UpdateType, EnterPVS );
		REG_ENUM( UpdateType, LeavePVS );
		REG_ENUM( UpdateType, DeltaEnt );
		REG_ENUM( UpdateType, PreserveEnt );
		REG_ENUM( UpdateType, Finished );
		REG_ENUM( UpdateType, Failed );
	END_ENUM_REGISTRATION();

	REG_GLBL_NUMBER( FHDR_ZERO );
	REG_GLBL_NUMBER( FHDR_LEAVEPVS );
	REG_GLBL_NUMBER( FHDR_DELETE );
	REG_GLBL_NUMBER( FHDR_ENTERPVS );

	REG_GLBL_STRING( INSTANCE_BASELINE_TABLENAME );
	REG_GLBL_STRING( LIGHT_STYLES_TABLENAME );
	REG_GLBL_STRING( USER_INFO_TABLENAME );
	REG_GLBL_STRING( SERVER_STARTUP_DATA_TABLENAME );

	REG_GLBL_NUMBER( NET_MESSAGE_BITS );

	REG_GLBL_NUMBER( net_NOP );
	REG_GLBL_NUMBER( net_Disconnect );
	REG_GLBL_NUMBER( net_File );
	REG_GLBL_NUMBER( net_LastControlMessage );

	REG_GLBL_NUMBER( net_Tick );
	REG_GLBL_NUMBER( net_StringCmd );
	REG_GLBL_NUMBER( net_SetConVar );
	REG_GLBL_NUMBER( net_SignonState );

	REG_GLBL_NUMBER( svc_ServerInfo );
	REG_GLBL_NUMBER( svc_SendTable );
	REG_GLBL_NUMBER( svc_ClassInfo );
	REG_GLBL_NUMBER( svc_SetPause );
	REG_GLBL_NUMBER( svc_CreateStringTable );
	REG_GLBL_NUMBER( svc_UpdateStringTable );
	REG_GLBL_NUMBER( svc_VoiceInit );
	REG_GLBL_NUMBER( svc_VoiceData );
	REG_GLBL_NUMBER( svc_Print );
	REG_GLBL_NUMBER( svc_Sounds );
	REG_GLBL_NUMBER( svc_SetView );
	REG_GLBL_NUMBER( svc_FixAngle );
	REG_GLBL_NUMBER( svc_CrosshairAngle );
	REG_GLBL_NUMBER( svc_BSPDecal );
	REG_GLBL_NUMBER( svc_UserMessage );
	REG_GLBL_NUMBER( svc_EntityMessage );
	REG_GLBL_NUMBER( svc_GameEvent );
	REG_GLBL_NUMBER( svc_PacketEntities );
	REG_GLBL_NUMBER( svc_TempEntities );
	REG_GLBL_NUMBER( svc_Prefetch );
	REG_GLBL_NUMBER( svc_Menu );
	REG_GLBL_NUMBER( svc_GameEventList );
	REG_GLBL_NUMBER( svc_GetCvarValue );
	REG_GLBL_NUMBER( svc_CmdKeyValues );
	REG_GLBL_NUMBER( svc_GMod_ServerToClient );
	REG_GLBL_NUMBER( SVC_LASTMSG );

	REG_GLBL_NUMBER( clc_ClientInfo );
	REG_GLBL_NUMBER( clc_Move );
	REG_GLBL_NUMBER( clc_VoiceData );
	REG_GLBL_NUMBER( clc_BaselineAck );
	REG_GLBL_NUMBER( clc_ListenEvents );
	REG_GLBL_NUMBER( clc_RespondCvarValue );
	REG_GLBL_NUMBER( clc_FileCRCCheck );
	REG_GLBL_NUMBER( clc_CmdKeyValues );
	REG_GLBL_NUMBER( clc_FileMD5Check );
	REG_GLBL_NUMBER( clc_GMod_ClientToServer );
	REG_GLBL_NUMBER( CLC_LASTMSG );

	REG_GLBL_NUMBER( RES_FATALIFMISSING );
	REG_GLBL_NUMBER( RES_PRELOAD );

	REG_GLBL_NUMBER( SIGNONSTATE_NONE );
	REG_GLBL_NUMBER( SIGNONSTATE_CHALLENGE );
	REG_GLBL_NUMBER( SIGNONSTATE_CONNECTED );
	REG_GLBL_NUMBER( SIGNONSTATE_NEW );
	REG_GLBL_NUMBER( SIGNONSTATE_PRESPAWN );
	REG_GLBL_NUMBER( SIGNONSTATE_SPAWN );
	REG_GLBL_NUMBER( SIGNONSTATE_FULL );
	REG_GLBL_NUMBER( SIGNONSTATE_CHANGELEVEL );

	REG_GLBL_NUMBER( MAX_STREAMS );
	REG_GLBL_NUMBER( FRAG_NORMAL_STREAM );
	REG_GLBL_NUMBER( FRAG_FILE_STREAM );

	REG_GLBL_NUMBER( MAX_RATE );
	REG_GLBL_NUMBER( MIN_RATE );
	REG_GLBL_NUMBER( DEFAULT_RATE );

	REG_GLBL_NUMBER( MAX_FRAGMENT_SIZE );
	REG_GLBL_NUMBER( MAX_SUBCHANNELS );
	REG_GLBL_NUMBER( MAX_FILE_SIZE );

	REG_GLBL_NUMBER( FLOW_OUTGOING );
	REG_GLBL_NUMBER( FLOW_INCOMING );
	REG_GLBL_NUMBER( MAX_FLOWS );

	REG_GLBL_NUMBER( MAX_CUSTOM_FILES );

	BEGIN_META_REGISTRATION( sn3_bf_write );
		REG_META_FUNCTION( sn3_bf_write, GetBasePointer );
		REG_META_FUNCTION( sn3_bf_write, GetMaxNumBits );
		REG_META_FUNCTION( sn3_bf_write, GetNumBitsWritten );
		REG_META_FUNCTION( sn3_bf_write, GetNumBytesWritten );
		REG_META_FUNCTION( sn3_bf_write, GetNumBitsLeft );
		REG_META_FUNCTION( sn3_bf_write, GetNumBytesLeft );

		REG_META_FUNCTION( sn3_bf_write, IsOverflowed );

		REG_META_FUNCTION( sn3_bf_write, Seek );

		REG_META_FUNCTION( sn3_bf_write, WriteBitAngle );
		REG_META_FUNCTION( sn3_bf_write, WriteBits );
		REG_META_FUNCTION( sn3_bf_write, WriteBitVec3Coord );
		REG_META_FUNCTION( sn3_bf_write, WriteByte );
		REG_META_FUNCTION( sn3_bf_write, WriteBytes );
		REG_META_FUNCTION( sn3_bf_write, WriteChar );
		REG_META_FUNCTION( sn3_bf_write, WriteFloat );
		REG_META_FUNCTION( sn3_bf_write, WriteLong );
		REG_META_FUNCTION( sn3_bf_write, WriteOneBit );
		REG_META_FUNCTION( sn3_bf_write, WriteShort );
		REG_META_FUNCTION( sn3_bf_write, WriteString );
		REG_META_FUNCTION( sn3_bf_write, WriteSBitLong );
		REG_META_FUNCTION( sn3_bf_write, WriteUBitLong );
		REG_META_FUNCTION( sn3_bf_write, WriteWord );

		REG_META_FUNCTION( sn3_bf_write, FinishWriting );
	END_META_REGISTRATION();

	REG_GLBL_FUNCTION( sn3_bf_write );

	BEGIN_META_REGISTRATION( sn3_bf_read );
		REG_META_FUNCTION( sn3_bf_read, GetBasePointer );
		REG_META_FUNCTION( sn3_bf_read, GetNumBitsLeft );
		REG_META_FUNCTION( sn3_bf_read, GetNumBytesLeft );
		REG_META_FUNCTION( sn3_bf_read, GetNumBitsRead );

		REG_META_FUNCTION( sn3_bf_read, IsOverflowed );

		REG_META_FUNCTION( sn3_bf_read, ReadBitAngle );
		REG_META_FUNCTION( sn3_bf_read, ReadBitAngles );
		REG_META_FUNCTION( sn3_bf_read, ReadBits );
		REG_META_FUNCTION( sn3_bf_read, ReadBitVec3Coord );
		REG_META_FUNCTION( sn3_bf_read, ReadByte );
		REG_META_FUNCTION( sn3_bf_read, ReadBytes );
		REG_META_FUNCTION( sn3_bf_read, ReadChar );
		REG_META_FUNCTION( sn3_bf_read, ReadFloat );
		REG_META_FUNCTION( sn3_bf_read, ReadLong );
		REG_META_FUNCTION( sn3_bf_read, ReadOneBit );
		REG_META_FUNCTION( sn3_bf_read, ReadShort );
		REG_META_FUNCTION( sn3_bf_read, ReadString );
		REG_META_FUNCTION( sn3_bf_read, ReadSBitLong );
		REG_META_FUNCTION( sn3_bf_read, ReadUBitLong );
		REG_META_FUNCTION( sn3_bf_read, ReadWord );

		REG_META_FUNCTION( sn3_bf_read, Seek );
		REG_META_FUNCTION( sn3_bf_read, SeekRelative );

		REG_META_FUNCTION( sn3_bf_read, TotalBytesAvailable );

		REG_META_FUNCTION( sn3_bf_read, FinishReading );
	END_META_REGISTRATION();

	REG_GLBL_FUNCTION( sn3_bf_read );

	BEGIN_META_REGISTRATION( CNetChan );
		REG_META_CALLBACK( CNetChan, __eq );

		REG_META_FUNCTION( CNetChan, DumpMessages );

		REG_META_FUNCTION( CNetChan, Reset );
		REG_META_FUNCTION( CNetChan, Clear );
		REG_META_FUNCTION( CNetChan, Shutdown );
		REG_META_FUNCTION( CNetChan, Transmit );

		REG_META_FUNCTION( CNetChan, SendFile );
		REG_META_FUNCTION( CNetChan, DenyFile );
		REG_META_FUNCTION( CNetChan, RequestFile );

		REG_META_FUNCTION( CNetChan, GetOutgoingQueueSize );
		REG_META_FUNCTION( CNetChan, GetOutgoingQueueFragments );
		REG_META_FUNCTION( CNetChan, QueueOutgoingFragments );

		REG_META_FUNCTION( CNetChan, GetIncomingFragments );

		REG_META_FUNCTION( CNetChan, GetSubChannels );

		REG_META_FUNCTION( CNetChan, GetReliableBuffer );
		REG_META_FUNCTION( CNetChan, GetUnreliableBuffer );
		REG_META_FUNCTION( CNetChan, GetVoiceBuffer );

		REG_META_FUNCTION( CNetChan, GetNetChannelHandler );
		REG_META_FUNCTION( CNetChan, GetAddress );
		REG_META_FUNCTION( CNetChan, GetTime );
		REG_META_FUNCTION( CNetChan, GetLatency );
		REG_META_FUNCTION( CNetChan, GetAvgLatency );
		REG_META_FUNCTION( CNetChan, GetAvgLoss );
		REG_META_FUNCTION( CNetChan, GetAvgChoke );
		REG_META_FUNCTION( CNetChan, GetAvgData );
		REG_META_FUNCTION( CNetChan, GetAvgPackets );
		REG_META_FUNCTION( CNetChan, GetTotalData );
		REG_META_FUNCTION( CNetChan, GetSequenceNr );
		REG_META_FUNCTION( CNetChan, IsValidPacket );
		REG_META_FUNCTION( CNetChan, GetPacketTime );
		REG_META_FUNCTION( CNetChan, GetPacketBytes );
		REG_META_FUNCTION( CNetChan, GetStreamProgress );
		REG_META_FUNCTION( CNetChan, GetCommandInterpolationAmount );
		REG_META_FUNCTION( CNetChan, GetPacketResponseLatency );
		REG_META_FUNCTION( CNetChan, GetRemoteFramerate );

		REG_META_FUNCTION( CNetChan, SetInterpolationAmount );
		REG_META_FUNCTION( CNetChan, SetRemoteFramerate );
		REG_META_FUNCTION( CNetChan, SetMaxBufferSize );

		REG_META_FUNCTION( CNetChan, IsPlayback );

		REG_META_FUNCTION( CNetChan, GetTimeoutSeconds );
		REG_META_FUNCTION( CNetChan, SetTimeoutSeconds );

		REG_META_FUNCTION( CNetChan, GetConnectTime );
		REG_META_FUNCTION( CNetChan, SetConnectTime );

		REG_META_FUNCTION( CNetChan, GetLastReceivedTime );
		REG_META_FUNCTION( CNetChan, SetLastReceivedTime );

		REG_META_FUNCTION( CNetChan, GetName );
		REG_META_FUNCTION( CNetChan, SetName );

		REG_META_FUNCTION( CNetChan, GetRate );
		REG_META_FUNCTION( CNetChan, SetRate );

		REG_META_FUNCTION( CNetChan, GetBackgroundMode );
		REG_META_FUNCTION( CNetChan, SetBackgroundMode );

		REG_META_FUNCTION( CNetChan, GetCompressionMode );
		REG_META_FUNCTION( CNetChan, SetCompressionMode );

		REG_META_FUNCTION( CNetChan, GetMaxRoutablePayloadSize );
		REG_META_FUNCTION( CNetChan, SetMaxRoutablePayloadSize );
	END_META_REGISTRATION();

	REG_GLBL_FUNCTION( CNetChan );

	BEGIN_META_REGISTRATION( subchannel_t );
		REG_META_FUNCTION( subchannel_t, GetFragmentOffset );
		REG_META_FUNCTION( subchannel_t, SetFragmentOffset );

		REG_META_FUNCTION( subchannel_t, GetFragmentNumber );
		REG_META_FUNCTION( subchannel_t, SetFragmentNumber );

		REG_META_FUNCTION( subchannel_t, GetSequence );
		REG_META_FUNCTION( subchannel_t, SetSequence );

		REG_META_FUNCTION( subchannel_t, GetState );
		REG_META_FUNCTION( subchannel_t, SetState );

		REG_META_FUNCTION( subchannel_t, GetIndex );
		REG_META_FUNCTION( subchannel_t, SetIndex );
	END_META_REGISTRATION();

	BEGIN_META_REGISTRATION( dataFragments_t );
		REG_META_FUNCTION( dataFragments_t, GetFileHandle );
		REG_META_FUNCTION( dataFragments_t, SetFileHandle );

		REG_META_FUNCTION( dataFragments_t, GetFileName );
		REG_META_FUNCTION( dataFragments_t, SetFileName );

		REG_META_FUNCTION( dataFragments_t, GetFileTransferID );
		REG_META_FUNCTION( dataFragments_t, SetFileTransferID );

		REG_META_FUNCTION( dataFragments_t, GetBuffer );
		REG_META_FUNCTION( dataFragments_t, SetBuffer );

		REG_META_FUNCTION( dataFragments_t, GetBytes );
		REG_META_FUNCTION( dataFragments_t, SetBytes );

		REG_META_FUNCTION( dataFragments_t, GetBits );
		REG_META_FUNCTION( dataFragments_t, SetBits );

		REG_META_FUNCTION( dataFragments_t, GetActualSize );
		REG_META_FUNCTION( dataFragments_t, SetActualSize );

		REG_META_FUNCTION( dataFragments_t, GetCompressed );
		REG_META_FUNCTION( dataFragments_t, SetCompressed );

		REG_META_FUNCTION( dataFragments_t, GetStream );
		REG_META_FUNCTION( dataFragments_t, SetStream );

		REG_META_FUNCTION( dataFragments_t, GetTotal );
		REG_META_FUNCTION( dataFragments_t, SetTotal );

		REG_META_FUNCTION( dataFragments_t, GetProgress );
		REG_META_FUNCTION( dataFragments_t, SetProgress );

		REG_META_FUNCTION( dataFragments_t, GetNum );
		REG_META_FUNCTION( dataFragments_t, SetNum );

		REG_META_FUNCTION( dataFragments_t, Delete );
	END_META_REGISTRATION();

	REG_GLBL_FUNCTION( dataFragments_t );

	BEGIN_META_REGISTRATION( FileHandle_t );
	END_META_REGISTRATION();

	BEGIN_META_REGISTRATION( UCHARPTR );
		REG_META_FUNCTION( UCHARPTR, Delete );
	END_META_REGISTRATION();

	REG_GLBL_FUNCTION( UCHARPTR );

	BEGIN_META_REGISTRATION( netadr_t );
		REG_META_FUNCTION( netadr_t, IsLocalhost );
		REG_META_FUNCTION( netadr_t, IsLoopback );
		REG_META_FUNCTION( netadr_t, IsReservedAdr );
		REG_META_FUNCTION( netadr_t, IsValid );

		REG_META_FUNCTION( netadr_t, GetIP );
		REG_META_FUNCTION( netadr_t, GetPort );
		REG_META_FUNCTION( netadr_t, GetType );

		REG_META_FUNCTION( netadr_t, ToString );
	END_META_REGISTRATION();

	BEGIN_META_REGISTRATION( INetworkStringTableContainer );
		REG_META_FUNCTION( INetworkStringTableContainer, FindTable );
		REG_META_FUNCTION( INetworkStringTableContainer, GetTable );
	END_META_REGISTRATION();

	REG_GLBL_FUNCTION( INetworkStringTableContainer );

	BEGIN_META_REGISTRATION( INetworkStringTable );
		REG_META_FUNCTION( INetworkStringTable, GetString );
	END_META_REGISTRATION();

	BEGIN_META_REGISTRATION( IGameEventManager2 );
		REG_META_FUNCTION( IGameEventManager2, CreateEvent );
		REG_META_FUNCTION( IGameEventManager2, SerializeEvent );
		REG_META_FUNCTION( IGameEventManager2, UnserializeEvent );
	END_META_REGISTRATION();

	REG_GLBL_FUNCTION( IGameEventManager2 );

	BEGIN_META_REGISTRATION( IGameEvent );
		REG_META_FUNCTION( IGameEvent, GetName );

		REG_META_FUNCTION( IGameEvent, IsReliable );
		REG_META_FUNCTION( IGameEvent, IsLocal );
		REG_META_FUNCTION( IGameEvent, IsEmpty );

		REG_META_FUNCTION( IGameEvent, GetBool );
		REG_META_FUNCTION( IGameEvent, GetInt );
		REG_META_FUNCTION( IGameEvent, GetFloat );
		REG_META_FUNCTION( IGameEvent, GetString );

		REG_META_FUNCTION( IGameEvent, SetBool );
		REG_META_FUNCTION( IGameEvent, SetInt );
		REG_META_FUNCTION( IGameEvent, SetFloat );
		REG_META_FUNCTION( IGameEvent, SetString );

		REG_META_FUNCTION( IGameEvent, Delete );
	END_META_REGISTRATION();

	REG_GLBL_FUNCTION( Attach__CNetChan_ProcessPacket );
	REG_GLBL_FUNCTION( Detach__CNetChan_ProcessPacket );

	REG_GLBL_FUNCTION( Attach__CNetChan_SendDatagram );
	REG_GLBL_FUNCTION( Detach__CNetChan_SendDatagram );

	REG_GLBL_FUNCTION( Attach__CNetChan_Shutdown );
	REG_GLBL_FUNCTION( Detach__CNetChan_Shutdown );

	REG_GLBL_FUNCTION( Attach__INetChannelHandler_ConnectionStart );
	REG_GLBL_FUNCTION( Detach__INetChannelHandler_ConnectionStart );

	REG_GLBL_FUNCTION( Attach__INetChannelHandler_ConnectionClosing );
	REG_GLBL_FUNCTION( Detach__INetChannelHandler_ConnectionClosing );
	
	REG_GLBL_FUNCTION( Attach__INetChannelHandler_ConnectionCrashed );
	REG_GLBL_FUNCTION( Detach__INetChannelHandler_ConnectionCrashed );
	
	REG_GLBL_FUNCTION( Attach__INetChannelHandler_PacketStart );
	REG_GLBL_FUNCTION( Detach__INetChannelHandler_PacketStart );

	REG_GLBL_FUNCTION( Attach__INetChannelHandler_PacketEnd );
	REG_GLBL_FUNCTION( Detach__INetChannelHandler_PacketEnd );

	REG_GLBL_FUNCTION( Attach__INetChannelHandler_FileRequested );
	REG_GLBL_FUNCTION( Detach__INetChannelHandler_FileRequested );

	REG_GLBL_FUNCTION( Attach__INetChannelHandler_FileReceived );
	REG_GLBL_FUNCTION( Detach__INetChannelHandler_FileReceived );

	REG_GLBL_FUNCTION( Attach__INetChannelHandler_FileDenied );
	REG_GLBL_FUNCTION( Detach__INetChannelHandler_FileDenied );

	REG_GLBL_FUNCTION( Attach__CNetChan_ProcessMessages );
	REG_GLBL_FUNCTION( Detach__CNetChan_ProcessMessages );

	REG_GLBL_FUNCTION( sourcenet_isDedicatedServer );

#ifdef SOURCENET_HOOKING
	CSimpleScan engineScn( fnEngineFactory );

	if ( !IS_ATTACHED( CNetChan_ProcessMessages ) )
	{
#ifdef _WIN32
		engineScn.Find( CNetChan_ProcessMessages_SIG, CNetChan_ProcessMessages_MSK, (void **)&CNetChan_ProcessMessages_T );
#else
		void *hEngine = dlopen( ENGINE_LIB, RTLD_NOW );
		
		if ( hEngine )
		{
			CNetChan_ProcessMessages_T = ResolveSymbol( hEngine, "_ZN8CNetChan15ProcessMessagesER7bf_read" );

			dlclose( hEngine );
		}
#endif
		if ( !CNetChan_ProcessMessages_T )
		{
			Lua()->ErrorNoHalt( "[gm_sourcenet3] Failed to locate CNetChan::ProcessMessages, report this!\n" );
		}
	}
	
	if ( !Lua()->IsClient() )
	{
		// Disables per-client threads (hacky fix for SendDatagram hooking)

		unsigned int ulNetThreadChunk;
		
		if ( engineScn.Find( NETCHUNK_SIG, NETCHUNK_MSK, (void **)&ulNetThreadChunk) )
		{
			ulNetThreadChunk += NETCHUNK_SIG_OFFSET;

			BEGIN_MEMEDIT( (void *)ulNetThreadChunk, NETPATCH_LEN );
				memcpy( (void *)ulNetThreadChunk, NETPATCH_NEW, NETPATCH_LEN );
			FINISH_MEMEDIT( (void *)ulNetThreadChunk, NETPATCH_LEN );

			g_bPatchedNetChunk = true;
		}
		else
		{
			g_bPatchedNetChunk = false;

			Lua()->Error( "[gm_sourcenet3] Failed to locate net thread chunk, report this!\n" );
		}
	}
#endif

	return 0;
}

// Module shutdown
int Close( lua_State *L )
{
	UsesLua();

	for ( int i = 0; i < g_LuaStates.Count(); i++ )
	{
		// State descriptor
		multiStateInfo msi = g_LuaStates[i];

		if ( msi.L == L )
		{
			// Free hook.Call reference
			Lua()->FreeReference( msi.ref_hook_Call );
			
			// Unregister state
			g_LuaStates.Remove( i );
			
			break;
		}
	}
#ifdef SOURCENET_HOOKING
	if ( !Lua()->IsClient() )
	{
		if ( g_bPatchedNetChunk )
		{
			CSimpleScan engineScn( fnEngineFactory );

			unsigned int ulNetThreadChunk;
			
			if ( engineScn.Find( NETCHUNK_SIG, NETCHUNK_MSK, (void **)&ulNetThreadChunk) )
			{
				ulNetThreadChunk += NETCHUNK_SIG_OFFSET;

				BEGIN_MEMEDIT( (void *)ulNetThreadChunk, NETPATCH_LEN );
					memcpy( (void *)ulNetThreadChunk, NETPATCH_OLD, NETPATCH_LEN );
				FINISH_MEMEDIT( (void *)ulNetThreadChunk, NETPATCH_LEN );
			}
		}
	}
#endif
	return 0;
}
