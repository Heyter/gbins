#include "FS.h"

namespace FS
{

IFileSystem *g_pFilesystem = NULL;

bool LoadFilesystem( )
{
	CreateInterfaceFn fsinterface = Sys_GetFactory( FILESYSTEM_STEAM_DLL );
    
	if( !fsinterface )
		return false;

	int nReturnCode = 0;
	g_pFilesystem = (IFileSystem *)fsinterface( FILESYSTEM_INTERFACE_VERSION, &nReturnCode );

	if( g_pFilesystem )
	{
		char pszSearchPath[512] = { 0 };
		sprintf( pszSearchPath, "garrysmod/%s", FILESYSTEM_JAIL_PATH );
		g_pFilesystem->AddSearchPath( pszSearchPath, "GAME" );
	}

	return true;
}

bool UnloadFilesystem( )
{
	if( g_pFilesystem )
	{
		char pszSearchPath[512] = { 0 };
		sprintf( pszSearchPath, "garrysmod/%s", FILESYSTEM_JAIL_PATH );
		g_pFilesystem->RemoveSearchPath( pszSearchPath, "GAME" );
	}

	return true;
}

}