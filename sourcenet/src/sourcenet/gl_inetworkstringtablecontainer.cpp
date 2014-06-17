#include "gl_inetworkstringtablecontainer.h"
#include "gl_inetworkstringtable.h"

#include <networkstringtabledefs.h>

META_ID( INetworkStringTableContainer, 10 );

META_FUNCTION( INetworkStringTableContainer, FindTable )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( INetworkStringTableContainer ) );
	Lua()->CheckType( 2, GLua::TYPE_STRING );

	INetworkStringTableContainer *pContainer = GET_META( 1, INetworkStringTableContainer );

	PUSH_META( pContainer->FindTable( Lua()->GetString( 2 ) ), INetworkStringTable );

	return 1;
}

META_FUNCTION( INetworkStringTableContainer, GetTable )
{
	UsesLua();

	//Lua()->SetGlobal( "META_ID" ,  (double)GET_META_ID( INetworkStringTableContainer ) );

	Lua()->CheckType( 1, GET_META_ID( INetworkStringTableContainer ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	INetworkStringTableContainer *pContainer = GET_META( 1, INetworkStringTableContainer );

	PUSH_META( pContainer->GetTable( Lua()->GetInteger( 2 ) ), INetworkStringTable );

	return 1;
}

GLBL_FUNCTION( INetworkStringTableContainer )
{
	UsesLua();

	INetworkStringTableContainer *pContainer = (INetworkStringTableContainer *)fnEngineFactory( Lua()->IsClient() ? INTERFACENAME_NETWORKSTRINGTABLECLIENT : INTERFACENAME_NETWORKSTRINGTABLESERVER, NULL );

	PUSH_META( pContainer, INetworkStringTableContainer );

	return 1;
}