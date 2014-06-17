#include "gl_igameeventmanager2.h"
#include "gl_igameevent.h"
#include "gl_bitbuf_read.h"
#include "gl_bitbuf_write.h"

#include <igameevents.h>

META_ID( IGameEventManager2, 12 );

META_FUNCTION( IGameEventManager2, CreateEvent )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEventManager2 ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	IGameEventManager2 *pGameEventListener = GET_META( 1, IGameEventManager2 );

	IGameEvent *event = pGameEventListener->CreateEvent( Lua()->GetString( 2 ), true );

	PUSH_META( event, IGameEvent );

	return 1;
}

META_FUNCTION( IGameEventManager2, SerializeEvent )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEventManager2 ) );
	Lua()->CheckType( 2, GET_META_ID( IGameEvent ) );
	Lua()->CheckType( 3, GET_META_ID( sn3_bf_write ) );

	IGameEventManager2 *pGameEventListener = GET_META( 1, IGameEventManager2 );

	IGameEvent *event = GET_META( 2, IGameEvent );

	sn3_bf_write *buf = GET_META( 3, sn3_bf_write );

	Lua()->Push( pGameEventListener->SerializeEvent( event, buf ) );

	return 1;
}

META_FUNCTION( IGameEventManager2, UnserializeEvent )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEventManager2 ) );
	Lua()->CheckType( 2, GET_META_ID( sn3_bf_read ) );

	IGameEventManager2 *pGameEventListener = GET_META( 1, IGameEventManager2 );

	sn3_bf_read *buf = GET_META( 2, sn3_bf_read );

	IGameEvent *event = pGameEventListener->UnserializeEvent( buf );

	PUSH_META( event, IGameEvent );

	return 1;
}

GLBL_FUNCTION( IGameEventManager2 )
{
	UsesLua();

	IGameEventManager2 *pGameEventListener = (IGameEventManager2 *)fnEngineFactory( INTERFACEVERSION_GAMEEVENTSMANAGER2, NULL );

	PUSH_META( pGameEventListener, IGameEventManager2 );

	return 1;
}