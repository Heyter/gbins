#include "gl_bitbuf_write.h"
#include "gl_ucharptr.h"

#include <bitbuf.h>

META_ID( sn3_bf_write, 1 );

META_FUNCTION( sn3_bf_write, GetBasePointer )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	PUSH_META( buf->GetBasePointer(), UCHARPTR );

	return 1;
}

META_FUNCTION( sn3_bf_write, GetMaxNumBits )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	Lua()->PushLong( buf->GetMaxNumBits() );

	return 1;
}

META_FUNCTION( sn3_bf_write, GetNumBitsWritten )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	Lua()->PushLong( buf->GetNumBitsWritten() );

	return 1;
}

META_FUNCTION( sn3_bf_write, GetNumBytesWritten )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	Lua()->PushLong( buf->GetNumBytesWritten() );

	return 1;
}

META_FUNCTION( sn3_bf_write, GetNumBitsLeft )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	Lua()->PushLong( buf->GetNumBitsLeft() );

	return 1;
}

META_FUNCTION( sn3_bf_write, GetNumBytesLeft )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	Lua()->PushLong( buf->GetNumBytesLeft() );

	return 1;
}

META_FUNCTION( sn3_bf_write, IsOverflowed )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	Lua()->Push( buf->IsOverflowed() );

	return 1;
}

META_FUNCTION( sn3_bf_write, Seek )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->SeekToBit( Lua()->GetInteger( 2 ) );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteBitAngle )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteBitAngle( Lua()->GetNumber( 2 ), Lua()->GetInteger( 3 ) );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteBits )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GET_META_ID( UCHARPTR ) );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	Lua()->Push( buf->WriteBits( GET_META( 2, UCHARPTR ), Lua()->GetInteger( 3 ) ) );

	return 1;
}

META_FUNCTION( sn3_bf_write, WriteBitVec3Coord )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_VECTOR );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteBitVec3Coord( *GET_META( 2, Vector ) );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteByte )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteByte( Lua()->GetInteger( 2 ) & 0xFF );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteBytes )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GET_META_ID( UCHARPTR ) );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	Lua()->Push( buf->WriteBytes( GET_META( 2, UCHARPTR ), Lua()->GetInteger( 3 ) ) );

	return 1;
}

META_FUNCTION( sn3_bf_write, WriteChar )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteChar( Lua()->GetInteger( 2 ) & 0xFF );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteFloat )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteFloat( Lua()->GetNumber( 2 ) );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteLong )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteLong( Lua()->GetInteger( 2 ) );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteOneBit )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteOneBit( Lua()->GetInteger( 2 ) );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteShort )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteShort( Lua()->GetInteger( 2 ) & 0xFFFF );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteString )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteString( Lua()->GetString( 2 ) );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteSBitLong )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteSBitLong( Lua()->GetInteger( 2 ), Lua()->GetInteger( 3 ) );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteUBitLong )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );
	Lua()->CheckType( 3, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteUBitLong( Lua()->GetInteger( 2 ), Lua()->GetInteger( 3 ) );

	return 0;
}

META_FUNCTION( sn3_bf_write, WriteWord )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_write *buf = GET_META( 1, sn3_bf_write );

	buf->WriteWord( Lua()->GetInteger( 2 ) );

	return 0;
}

META_FUNCTION( sn3_bf_write, FinishWriting )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_write ) );

	delete GET_META( 1, sn3_bf_write );

	return 0;
}

GLBL_FUNCTION( sn3_bf_write )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( UCHARPTR ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	int bits = Lua()->GetInteger( 2 );

	sn3_bf_write *buf = new sn3_bf_write( (void *)GET_META( 1, UCHARPTR ), BitByte( bits ), bits );

	PUSH_META( buf, sn3_bf_write );

	return 1;
}