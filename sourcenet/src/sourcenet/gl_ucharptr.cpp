#include "gl_ucharptr.h"

META_ID( UCHARPTR, 7 );

META_FUNCTION( UCHARPTR, Delete )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( UCHARPTR ) );

	unsigned char *ptr = (unsigned char *)GET_META( 1, UCHARPTR );

	delete[] ptr;

	return 0;
}

GLBL_FUNCTION( UCHARPTR )
{
	UsesLua();

	Lua()->CheckType( 1, GLua::TYPE_NUMBER );

	unsigned char *ptr = new unsigned char[ Lua()->GetInteger( 1 ) ];

	PUSH_META( ptr, UCHARPTR );

	return 1;
}