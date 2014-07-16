#include "FS.h"

namespace FS
{

IFileSystem *g_pFilesystem = NULL;

void *CreateInterface( const char *pName, int *pReturnCode )
{
	if( pReturnCode )
		*pReturnCode = IFACE_FAILED;

	return NULL;
}

bool LoadFilesystem( )
{
	CreateInterfaceFn _CreateInterface = Sys_GetFactory( FILESYSTEM_STEAM_DLL );
	
	if( !_CreateInterface )
		return false;

	int nReturnCode = 0;
	g_pFilesystem = (IFileSystem *)_CreateInterface( FILESYSTEM_INTERFACE_VERSION, &nReturnCode );


	if( g_pFilesystem )
	{
		g_pFilesystem->Connect( CreateInterface );

		// TODO: Figure out if do we need to call Init?
		// apparently it works without, so leave it alone.
		// g_pFilesystem->Init();

		char pszSearchPath[512] = { 0 };
		sprintf( pszSearchPath, "garrysmod/%s", FILESYSTEM_JAIL_PATH );

		g_pFilesystem->AddSearchPath( pszSearchPath, "GAME" );
	}

	return true;
}

bool UnloadFilesystem( )
{
	if( g_pFilesystem )
		g_pFilesystem->Disconnect( );

	return true;
}

}