#include "gl_datafragments_t.h"
#include "gl_filehandle_t.h"
#include "gl_ucharptr.h"

META_ID( dataFragments_t, 5 );

META_FUNCTION( dataFragments_t, GetFileHandle )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	PUSH_META( fragments->hfile, FileHandle_t );

	return 1;
}

META_FUNCTION( dataFragments_t, SetFileHandle )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	CopyUserDataOrNull( 2, FileHandle_t, fragments->hfile, FileHandle_t );

	return 0;
}

META_FUNCTION( dataFragments_t, GetFileName )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	Lua()->Push( (const char *)fragments->filename );

	return 1;
}

META_FUNCTION( dataFragments_t, SetFileName )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );
	Lua()->CheckType( 2 , GLua::TYPE_STRING );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	strcpy_s( fragments->filename, MAX_PATH, Lua()->GetString( 2 ) );

	return 0;
}

META_FUNCTION( dataFragments_t, GetFileTransferID )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	Lua()->PushLong( fragments->transferid );

	return 1;
}

META_FUNCTION( dataFragments_t, SetFileTransferID )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	fragments->transferid = Lua()->GetInteger( 2 );

	return 0;
}

META_FUNCTION( dataFragments_t, GetBuffer )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	PUSH_META( fragments->buffer, UCHARPTR );

	return 1;
}

META_FUNCTION( dataFragments_t, SetBuffer )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );
	
	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	CopyUserDataOrNull( 2, UCHARPTR, fragments->buffer, unsigned char * );
		
	return 0;
}


META_FUNCTION( dataFragments_t, GetBytes )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	Lua()->PushLong( fragments->bytes );

	return 1;
}

META_FUNCTION( dataFragments_t, SetBytes )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	fragments->bytes = Lua()->GetInteger( 2 );

	return 0;
}

META_FUNCTION( dataFragments_t, GetBits )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	Lua()->PushLong( fragments->bits );

	return 1;
}

META_FUNCTION( dataFragments_t, SetBits )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	fragments->bits = Lua()->GetInteger( 2 );

	return 0;
}

META_FUNCTION( dataFragments_t, GetActualSize )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	Lua()->PushLong( fragments->actualsize );

	return 1;
}

META_FUNCTION( dataFragments_t, SetActualSize )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	fragments->actualsize = Lua()->GetInteger( 2 );

	return 0;
}

META_FUNCTION( dataFragments_t, GetCompressed )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	Lua()->Push( fragments->compressed );

	return 1;
}

META_FUNCTION( dataFragments_t, SetCompressed )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );
	Lua()->CheckType( 2 , GLua::TYPE_BOOL );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	fragments->compressed = Lua()->GetBool( 2 );

	return 0;
}

META_FUNCTION( dataFragments_t, GetStream )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	Lua()->Push( fragments->stream );

	return 1;
}

META_FUNCTION( dataFragments_t, SetStream )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );
	Lua()->CheckType( 2 , GLua::TYPE_BOOL );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	fragments->stream = Lua()->GetBool( 2 );

	return 0;
}

META_FUNCTION( dataFragments_t, GetTotal )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	Lua()->PushLong( fragments->total );

	return 1;
}

META_FUNCTION( dataFragments_t, SetTotal )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	fragments->total = Lua()->GetInteger( 2 );

	return 0;
}

META_FUNCTION( dataFragments_t, GetProgress )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	Lua()->PushLong( fragments->progress );

	return 1;
}

META_FUNCTION( dataFragments_t, SetProgress )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	fragments->progress = Lua()->GetInteger( 2 );

	return 0;
}

META_FUNCTION( dataFragments_t, GetNum )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	Lua()->PushLong( fragments->num );

	return 1;
}

META_FUNCTION( dataFragments_t, SetNum )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );
	Lua()->CheckType( 2, GLua::TYPE_NUMBER );

	dataFragments_t *fragments = GET_META( 1, dataFragments_t );

	fragments->num = Lua()->GetInteger( 2 );

	return 0;
}

META_FUNCTION( dataFragments_t, Delete )
{
	UsesLua();

	Lua()->CheckType( 1, GET_META_ID( dataFragments_t ) );

	delete GET_META( 1, dataFragments_t );

	return 0;
}

GLBL_FUNCTION( dataFragments_t )
{
	UsesLua();

	dataFragments_t *fragments = new dataFragments_t;

	PUSH_META( fragments, dataFragments_t );

	return 1;
}