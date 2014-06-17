#include "gl_igameevent.h"

#include <igameevents.h>

META_ID( IGameEvent, 13 );

META_FUNCTION( IGameEvent, GetName )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );

	IGameEvent *event = GET_META( 1, IGameEvent );

	Lua()->Push( event->GetName() );

	return 1;
}

META_FUNCTION( IGameEvent, IsReliable )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );

	IGameEvent *event = GET_META( 1, IGameEvent );

	Lua()->Push( event->IsReliable() );

	return 1;
}

META_FUNCTION( IGameEvent, IsLocal )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );

	IGameEvent *event = GET_META( 1, IGameEvent );

	Lua()->Push( event->IsLocal() );

	return 1;
}

META_FUNCTION( IGameEvent, IsEmpty )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );

	IGameEvent *event = GET_META( 1, IGameEvent );

	Lua()->Push( event->IsEmpty( Lua()->GetString( 2 ) ) );

	return 1;
}

META_FUNCTION( IGameEvent, GetBool )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	IGameEvent *event = GET_META( 1, IGameEvent );

	Lua()->Push( event->GetBool( Lua()->GetString( 2 ) ) );

	return 1;
}

META_FUNCTION( IGameEvent, GetInt )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	IGameEvent *event = GET_META( 1, IGameEvent );

	Lua()->PushLong( event->GetInt( Lua()->GetString( 2 ) ) );

	return 1;
}

META_FUNCTION( IGameEvent, GetFloat )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	IGameEvent *event = GET_META( 1, IGameEvent );

	Lua()->Push( event->GetFloat( Lua()->GetString( 2 ) ) );

	return 1;
}

META_FUNCTION( IGameEvent, GetString )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	IGameEvent *event = GET_META( 1, IGameEvent );

	Lua()->Push( event->GetString( Lua()->GetString( 2 ) ) );

	return 1;
}

META_FUNCTION( IGameEvent, SetBool )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );
	Lua()->CheckType( 3, GLua::TYPE_BOOL );

	IGameEvent *event = GET_META( 1, IGameEvent );

	event->SetBool( Lua()->GetString( 2 ), Lua()->GetBool( 3 ) );

	return 0;
}

META_FUNCTION( IGameEvent, SetInt )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	IGameEvent *event = GET_META( 1, IGameEvent );

	event->SetInt( Lua()->GetString( 2 ), Lua()->GetInteger( 3 ) );

	return 0;
}

META_FUNCTION( IGameEvent, SetFloat )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	IGameEvent *event = GET_META( 1, IGameEvent );

	event->SetFloat( Lua()->GetString( 2 ), Lua()->GetNumber( 3 ) );

	return 0;
}

META_FUNCTION( IGameEvent, SetString )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );
	Lua()->CheckType( 3, GLua::TYPE_STRING );

	IGameEvent *event = GET_META( 1, IGameEvent );

	event->SetString( Lua()->GetString( 2 ), Lua()->GetString( 3 ) );

	return 0;
}

META_FUNCTION( IGameEvent, Delete )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( IGameEvent ) );

	IGameEvent *event = GET_META( 1, IGameEvent );

	event->~IGameEvent();

	return 0;
}