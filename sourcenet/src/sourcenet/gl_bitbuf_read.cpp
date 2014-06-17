#include "gl_bitbuf_read.h"
#include "gl_ucharptr.h"

#include <bitbuf.h>

META_ID( sn3_bf_read, 2 );

META_FUNCTION( sn3_bf_read, GetBasePointer )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	PUSH_META( buf->GetBasePointer(), UCHARPTR );

	return 1;
}

META_FUNCTION( sn3_bf_read, GetNumBitsLeft )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->GetNumBitsLeft() );

	return 1;
}

META_FUNCTION( sn3_bf_read, GetNumBytesLeft )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->GetNumBytesLeft() );

	return 1;
}

META_FUNCTION( sn3_bf_read, GetNumBitsRead )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->GetNumBitsRead() );

	return 1;
}

META_FUNCTION( sn3_bf_read, IsOverflowed )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->Push( buf->IsOverflowed() );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadBitAngle )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->Push( buf->ReadBitAngle( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadBitAngles )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	QAngle *ang = new QAngle;

	buf->ReadBitAngles( *ang );

	ILuaObject *Angle__MT = Lua()->GetMetaTable( "Angle", GLua::TYPE_ANGLE );
	
	Lua()->PushUserData( Angle__MT, ang );

	Angle__MT->UnReference();

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadBits )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	int Int = BitByte( Lua()->GetInteger( 2 ) );
	unsigned char *data = new unsigned char[ Int ];
	if ( !data )
	{
		Lua()->Error( "[gm_sourcenet3] Failed to allocate " + Int );
		//Lua()->Msg( "[gm_sourcenet3][sn3_bf_read::ReadBits] Failed allocating %i bytes\n", BitByte( Lua()->GetInteger( 2 ) ) );
		Lua()->Error( "[gm_sourcenet3][sn3_bf_read::ReadBits] Fatal error" );

		// Prevent further reading of the buffer
		buf->SetOverflowFlag();
	}

	buf->ReadBits( data, Lua()->GetInteger( 2 ) );

	PUSH_META( data, UCHARPTR );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadBitVec3Coord )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Vector *vec = new Vector;

	buf->ReadBitVec3Coord( *vec );

	ILuaObject *Vector__MT = Lua()->GetMetaTable( "Vector", GLua::TYPE_VECTOR );
	
	Lua()->PushUserData( Vector__MT, vec );

	Vector__MT->UnReference();

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadByte )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->ReadByte() );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadBytes )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	unsigned char *data = new unsigned char[ Lua()->GetInteger( 2 ) ];

	if ( !data )
	{
		Lua()->Msg( "[gm_sourcenet3][sn3_bf_read::ReadBytes] Failed allocating %i bytes\n", Lua()->GetInteger( 2 ) );
		Lua()->Error( "[gm_sourcenet3][sn3_bf_read::ReadBytes] Fatal error" );

		// Prevent further reading of the buffer
		buf->SetOverflowFlag();
	}

	buf->ReadBytes( data, Lua()->GetInteger( 2 ) );

	PUSH_META( data, UCHARPTR );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadChar )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->ReadChar() );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadFloat )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->Push( buf->ReadFloat() );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadLong )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->ReadLong() );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadOneBit )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->ReadOneBit() );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadShort )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->ReadShort() );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadString )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	char str[ 1024 ];

	if ( buf->ReadString( str, 1024 ) )
		Lua()->Push( (const char *)str );
	else
		Lua()->PushNil();

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadSBitLong )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->ReadSBitLong( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadUBitLong )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->ReadUBitLong( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( sn3_bf_read, ReadWord )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->ReadWord() );

	return 1;
}

META_FUNCTION( sn3_bf_read, Seek )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->Push( buf->Seek( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( sn3_bf_read, SeekRelative )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->Push( buf->SeekRelative( Lua()->GetInteger( 2 ) ) );

	return 1;
}

META_FUNCTION( sn3_bf_read, TotalBytesAvailable )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	sn3_bf_read *buf = GET_META( 1, sn3_bf_read );

	Lua()->PushLong( buf->TotalBytesAvailable() );

	return 1;
}

META_FUNCTION( sn3_bf_read, FinishReading )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( sn3_bf_read ) );

	delete GET_META( 1, sn3_bf_read );

	return 0;
}

GLBL_FUNCTION( sn3_bf_read )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( UCHARPTR ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	int bits = Lua()->GetInteger( 2 );

	sn3_bf_read *buf = new sn3_bf_read( (void *)GET_META( 1, UCHARPTR ), BitByte( bits ), bits );

	PUSH_META( buf, sn3_bf_read );

	return 1;
}