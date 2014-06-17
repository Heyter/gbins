#include "gl_netadr_t.h"

#include <netadr.h>

META_ID( netadr_t, 8 );

META_FUNCTION( netadr_t, IsLocalhost )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( netadr_t ) );

	netadr_t *adr = GET_META( 1, netadr_t );

	Lua()->Push( adr->IsLocalhost() );

	return 1;
}

META_FUNCTION( netadr_t, IsLoopback )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( netadr_t ) );

	netadr_t *adr = GET_META( 1, netadr_t );

	Lua()->Push( adr->IsLoopback() );

	return 1;
}

META_FUNCTION( netadr_t, IsReservedAdr )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( netadr_t ) );

	netadr_t *adr = GET_META( 1, netadr_t );

	Lua()->Push( adr->IsReservedAdr() );

	return 1;
}

META_FUNCTION( netadr_t, IsValid )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( netadr_t ) );

	netadr_t *adr = GET_META( 1, netadr_t );

	Lua()->Push( adr->IsValid() );

	return 1;
}

META_FUNCTION( netadr_t, GetIP )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( netadr_t ) );

	netadr_t *adr = GET_META( 1, netadr_t );

	Lua()->PushLong( adr->GetIPHostByteOrder() );
	Lua()->PushLong( adr->GetIPNetworkByteOrder() );

	return 2;
}

META_FUNCTION( netadr_t, GetPort )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( netadr_t ) );

	netadr_t *adr = GET_META( 1, netadr_t );

	Lua()->PushLong( adr->GetPort() );

	return 1;
}

META_FUNCTION( netadr_t, GetType )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( netadr_t ) );

	netadr_t *adr = GET_META( 1, netadr_t );

	Lua()->PushLong( adr->GetType() );

	return 1;
}

META_FUNCTION( netadr_t, ToString )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( netadr_t ) );

	netadr_t *adr = GET_META( 1, netadr_t );

	Lua()->Push( adr->ToString( Lua()->GetBool( 2 ) ) );

	return 1;
}