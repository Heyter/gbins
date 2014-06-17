#define IVENGINESERVER_INTERFACE
#define IVENGINECLIENT_INTERFACE

#include "gl_cnetchan.h"
#include "gl_inetchannelhandler.h"
#include "gl_subchannel_t.h"
#include "gl_datafragments_t.h"
#include "gl_bitbuf_write.h"
#include "gl_netadr_t.h"

#include "net.h"

#include <inetmessage.h>

META_ID( CNetChan, 3 );

META_FUNCTION( CNetChan, DumpMessages )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	for ( int i = 0; i < netchan->netmessages.Count(); i++)
	{
		INetMessage *netmsg = netchan->netmessages.Element( i );

		Msg( "%d. %s (%d)\n", i + 1, netmsg->GetName(), netmsg->GetType() );
	}

	return 0;
}

META_FUNCTION( CNetChan, Reset )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->Reset();

	return 0;
}

META_FUNCTION( CNetChan, Clear )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->Clear();

	return 0;
}

META_FUNCTION( CNetChan, Shutdown )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->Shutdown( Lua()->GetString( 2 ) );

	return 0;
}

META_FUNCTION( CNetChan, Transmit )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	bool onlyReliable = false;

	if ( Lua()->GetType( 2 ) == GLua::TYPE_BOOL )
		onlyReliable = Lua()->GetBool( 2 );

	Lua()->Push( netchan->Transmit( onlyReliable ) );

	return 1;
}

META_FUNCTION( CNetChan, SendFile )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->SendFile( Lua()->GetString( 2 ), Lua()->GetInteger( 3 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, DenyFile )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->DenyFile( Lua()->GetString( 2 ), Lua()->GetInteger( 3 ) );

	return 0;
}

META_FUNCTION( CNetChan, RequestFile )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->PushLong( netchan->RequestFile( Lua()->GetString( 2 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, GetOutgoingQueueSize )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	int stream = Lua()->GetInteger( 2 );

	VerifyStream( stream );

	Lua()->PushLong( netchan->waitlist[ stream ].Count() );

	return 1;
}

META_FUNCTION( CNetChan, GetOutgoingQueueFragments )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	int stream = Lua()->GetInteger( 2 );

	VerifyStream( stream );

	int offset = Lua()->GetInteger( 3 );

	VerifyOffset( netchan->waitlist[ stream ], offset );

	PUSH_META( netchan->waitlist[ stream ].Element( offset ), dataFragments_t );

	return 1;
}

META_FUNCTION( CNetChan, QueueOutgoingFragments )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GET_META_ID( dataFragments_t ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	int stream = Lua()->GetInteger( 2 );

	VerifyStream( stream );

	dataFragments_t *fragments = GET_META( 3, dataFragments_t );

	netchan->waitlist[ stream ].AddToTail( fragments );

	return 0;
}

META_FUNCTION( CNetChan, GetIncomingFragments )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	int stream = Lua()->GetInteger( 2 );

	VerifyStream( stream );

	PUSH_META( &netchan->recvlist[ stream ], dataFragments_t );

	return 1;
}

META_FUNCTION( CNetChan, GetSubChannels )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	ILuaObject *subchannels_tbl = Lua()->GetNewTable();

	for ( int i = 0; i < MAX_SUBCHANNELS; i++ )
	{
		subchannel_t *subchan = &netchan->subchannels[i];

		ILuaObject *subchan_meta = Lua()->GetMetaTable( GET_META_NAME( subchannel_t ), GET_META_ID( subchannel_t ) );
		
		Lua()->PushUserData( subchan_meta, subchan ); // Push subchan onto stack as userdata
		
		subchan_meta->UnReference();
		
		ILuaObject *subchan_obj = Lua()->GetObject(); // Pop as ILuaObject
		
		subchannels_tbl->SetMember( (float)i + 1, subchan_obj );
		
		subchan_obj->UnReference();
	}

	subchannels_tbl->Push();
	subchannels_tbl->UnReference();

	return 1;
}

META_FUNCTION( CNetChan, GetReliableBuffer )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	PUSH_META( &netchan->reliabledata, sn3_bf_write );

	return 1;
}

META_FUNCTION( CNetChan, GetUnreliableBuffer )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	PUSH_META( &netchan->unreliabledata, sn3_bf_write );

	return 1;
}

META_FUNCTION( CNetChan, GetVoiceBuffer )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	PUSH_META( &netchan->voicedata, sn3_bf_write );

	return 1;
}

META_FUNCTION( CNetChan, GetNetChannelHandler )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	PUSH_META( netchan->GetMsgHandler(), INetChannelHandler );

	return 1;
}

META_FUNCTION( CNetChan, GetAddress )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	PUSH_META( &netchan->remote_address, netadr_t );
	
	return 1;
}

META_FUNCTION( CNetChan, GetTime )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->GetTime() );

	return 1;
}

META_FUNCTION( CNetChan, GetLatency )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->GetLatency( Lua()->GetInteger( 2 ) ) );

	return 1;
}

EXT_META_FUNCTION( CNetChan, GetAvgLatency )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->GetAvgLatency( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, GetAvgLoss )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->GetAvgLoss( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, GetAvgChoke )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->GetAvgChoke( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, GetAvgData )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->GetAvgData( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, GetAvgPackets )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->GetAvgPackets( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, GetTotalData )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->PushLong( netchan->GetTotalData( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, GetSequenceNr )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->PushLong( netchan->GetSequenceNr( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, IsValidPacket )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->IsValidPacket( Lua()->GetInteger( 2 ), Lua()->GetInteger( 3 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, GetPacketTime )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->GetPacketTime( Lua()->GetInteger( 2 ), Lua()->GetInteger( 3 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, GetPacketBytes )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );
	Lua()->CheckType( 4, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->PushLong( netchan->GetPacketBytes( Lua()->GetInteger( 2 ), Lua()->GetInteger( 3 ), Lua()->GetInteger( 4 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, GetStreamProgress )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	int received, total;

	if ( netchan->GetStreamProgress( Lua()->GetInteger( 2 ), &received, &total ) )
	{
		Lua()->PushLong( received );
		Lua()->PushLong( total );
	}
	else
	{
		Lua()->PushNil();
		Lua()->PushNil();
	}

	return 2;
}

META_FUNCTION( CNetChan, GetCommandInterpolationAmount )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->GetCommandInterpolationAmount( Lua()->GetInteger( 2 ), Lua()->GetInteger( 3 ) ) );

	return 1;
}

META_FUNCTION( CNetChan, GetPacketResponseLatency )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	int latencymsecs, choke;

	netchan->GetPacketResponseLatency( Lua()->GetInteger( 2 ), Lua()->GetInteger( 3 ), &latencymsecs, &choke );

	Lua()->PushLong( latencymsecs );
	Lua()->PushLong( choke );

	return 2;
}

META_FUNCTION( CNetChan, GetRemoteFramerate )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	float frametime, frametimedev;

	netchan->GetRemoteFramerate( &frametime, &frametimedev );

	Lua()->Push( frametime );
	Lua()->Push( frametimedev );

	return 2;
}

META_FUNCTION( CNetChan, SetInterpolationAmount )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->SetInterpolationAmount( Lua()->GetNumber( 2 ) );

	return 0;
}

META_FUNCTION( CNetChan, SetRemoteFramerate )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->SetRemoteFramerate( Lua()->GetNumber( 2 ), Lua()->GetNumber( 3 ) );

	return 0;
}

META_FUNCTION( CNetChan, SetMaxBufferSize )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_BOOL );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );
	Lua()->CheckType( 4, GLua::TYPE_BOOL );
	
	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->SetMaxBufferSize( Lua()->GetBool( 2 ), Lua()->GetInteger( 3 ), Lua()->GetBool( 4 ) );

	return 0;
}

META_FUNCTION( CNetChan, IsPlayback )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->IsPlayback() );

	return 1;
}

META_FUNCTION( CNetChan, GetTimeoutSeconds )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->timeout_seconds );

	return 1;
}

META_FUNCTION( CNetChan, SetTimeoutSeconds )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->timeout_seconds = Lua()->GetNumber( 2 );

	return 1;
}

META_FUNCTION( CNetChan, GetConnectTime )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->connect_time );

	return 1;
}

META_FUNCTION( CNetChan, SetConnectTime )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->connect_time = Lua()->GetNumber( 2 );

	return 1;
}

META_FUNCTION( CNetChan, GetLastReceivedTime )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->last_received );

	return 1;
}

META_FUNCTION( CNetChan, SetLastReceivedTime )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->last_received = Lua()->GetNumber( 2 );

	return 0;
}

META_FUNCTION( CNetChan, GetName )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->name );

	return 1;
}

META_FUNCTION( CNetChan, SetName )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	CNetChan *netchan = GET_META( 1, CNetChan );

	strcpy_s( netchan->name, 32, Lua()->GetString( 2 ) );

	return 0;
}

META_FUNCTION( CNetChan, GetRate )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );
	
	Lua()->PushLong( netchan->rate );

	return 1;
}

META_FUNCTION( CNetChan, SetRate )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->rate = Lua()->GetInteger( 2 );

	return 0;
}

META_FUNCTION( CNetChan, GetBackgroundMode )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->backgroundmode );

	return 1;
}

META_FUNCTION( CNetChan, SetBackgroundMode )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_BOOL );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->backgroundmode = Lua()->GetBool( 2 );

	return 0;
}

META_FUNCTION( CNetChan, GetCompressionMode )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->Push( netchan->usecompression );

	return 1;
}

META_FUNCTION( CNetChan, SetCompressionMode )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_BOOL );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->usecompression = Lua()->GetBool( 2 );

	return 0;
}

META_FUNCTION( CNetChan, GetMaxRoutablePayloadSize )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );

	CNetChan *netchan = GET_META( 1, CNetChan );

	Lua()->PushLong( netchan->splitsize );

	return 1;
}

META_FUNCTION( CNetChan, SetMaxRoutablePayloadSize )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( CNetChan ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	CNetChan *netchan = GET_META( 1, CNetChan );

	netchan->SetMaxRoutablePayloadSize( Lua()->GetInteger( 2 ) );

	return 0;
}

META_FUNCTION( CNetChan, __eq )
{
	UsesLua();

	CNetChan *netchan1 = (CNetChan *)Lua()->GetUserData( 1 );
	CNetChan *netchan2 = (CNetChan *)Lua()->GetUserData( 2 );

	Lua()->Push( netchan1 == netchan2 );

	return 1;
}

GLBL_FUNCTION( CNetChan )
{
	UsesLua();

	CNetChan *netchan = NULL;

	if ( !false )
	{
		Lua()->CheckType( 1, GLua::TYPE_NUMBER );

		netchan = (CNetChan *)g_pEngineServer->GetPlayerNetInfo( Lua()->GetInteger( 1 ) );	
	}
	else
	{
		netchan = (CNetChan *)g_pEngineClient->GetNetChannelInfo();
	}

	if ( netchan )
	{
		PUSH_META( netchan, CNetChan );
	}
	else
	{
		Lua()->PushNil();
	}

	return 1;
}