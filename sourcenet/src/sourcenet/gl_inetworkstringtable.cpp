#include "gl_inetworkstringtable.h"

#include <networkstringtabledefs.h>

META_ID( INetworkStringTable, 11 );

META_FUNCTION( INetworkStringTable, GetString )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( INetworkStringTable ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	INetworkStringTable *pTable = GET_META( 1, INetworkStringTable );

	const char *str = pTable->GetString( Lua()->GetInteger( 2 ) );

	if ( str )
		Lua()->Push( str );
	else
		Lua()->PushNil();

	return 1;
}