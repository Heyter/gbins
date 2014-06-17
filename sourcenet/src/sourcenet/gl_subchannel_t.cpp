#include "gl_subchannel_t.h"

#include "net.h"

META_ID( subchannel_t, 4 );

META_FUNCTION( subchannel_t, GetFragmentOffset )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( subchannel_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	subchannel_t *subchan = GET_META( 1, subchannel_t );

	int stream = Lua()->GetInteger( 2 );

	VerifyStream( stream );

	Lua()->PushLong( subchan->frag_ofs[ stream ] );

	return 1;
}

META_FUNCTION( subchannel_t, SetFragmentOffset )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( subchannel_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	subchannel_t *subchan = GET_META( 1, subchannel_t );

	int stream = Lua()->GetInteger( 2 );

	VerifyStream( stream );

	subchan->frag_ofs[ stream ] = Lua()->GetInteger( 3 );

	return 0;
}

META_FUNCTION( subchannel_t, GetFragmentNumber )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( subchannel_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	subchannel_t *subchan = GET_META( 1, subchannel_t );

	int stream = Lua()->GetInteger( 2 );

	VerifyStream( stream );

	Lua()->PushLong( subchan->frag_num[ stream ] );

	return 1;
}

META_FUNCTION( subchannel_t, SetFragmentNumber )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( subchannel_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	subchannel_t *subchan = GET_META( 1, subchannel_t );

	int stream = Lua()->GetInteger( 2 );

	VerifyStream( stream );

	subchan->frag_num[ stream ] = Lua()->GetInteger( 3 );

	return 0;
}

META_FUNCTION( subchannel_t, GetSequence )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( subchannel_t ) );

	subchannel_t *subchan = GET_META( 1, subchannel_t );

	Lua()->PushLong( subchan->sequence );

	return 1;
}

META_FUNCTION( subchannel_t, SetSequence )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( subchannel_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	subchannel_t *subchan = GET_META( 1, subchannel_t );

	subchan->sequence = Lua()->GetInteger( 2 );

	return 1;
}

META_FUNCTION( subchannel_t, GetState )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( subchannel_t ) );

	subchannel_t *subchan = GET_META( 1, subchannel_t );

	Lua()->PushLong( subchan->state );

	return 1;
}

META_FUNCTION( subchannel_t, SetState )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( subchannel_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	subchannel_t *subchan = GET_META( 1, subchannel_t );

	subchan->state = Lua()->GetInteger( 2 );

	return 1;
}

META_FUNCTION( subchannel_t, GetIndex )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( subchannel_t ) );

	subchannel_t *subchan = GET_META( 1, subchannel_t );

	Lua()->PushLong( subchan->index );

	return 1;
}

META_FUNCTION( subchannel_t, SetIndex )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( subchannel_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	subchannel_t *subchan = GET_META( 1, subchannel_t );

	subchan->index = Lua()->GetInteger( 2 );

	return 1;
}