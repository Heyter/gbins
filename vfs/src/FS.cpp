#include "FS.h"

namespace FS
{

IFileSystem *g_pFilesystem = NULL;

bool LoadFilesystem( )
{
	CreateInterfaceFn fsinterface = Sys_GetFactory( FILESYSTEM_STEAM_DLL );
	if( !fsinterface )
		return false;

	g_pFilesystem = (IFileSystem *)fsinterface( FILESYSTEM_INTERFACE_VERSION, NULL );
	if( g_pFilesystem != NULL )
	{
		g_pFilesystem->AddSearchPath( "garrysmod/" FILESYSTEM_JAIL_PATH, "GAME" );
		return true;
	}

	return false;
}

bool UnloadFilesystem( )
{
	if( g_pFilesystem != NULL )
	{
		g_pFilesystem->RemoveSearchPath( "garrysmod/" FILESYSTEM_JAIL_PATH, "GAME" );
		return true;
	}

	return false;
}

}