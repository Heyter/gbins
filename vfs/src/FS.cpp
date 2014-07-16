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
		
	CSysModule* FileSystemFactoryDLL = NULL;
	if (Sys_LoadInterface(FILESYSTEM_STEAM_DLL, FILESYSTEM_INTERFACE_VERSION, &FileSystemFactoryDLL, (void**)&g_pFilesystem)) {
		
	} else {
		return false;
	}

	if( g_pFilesystem )
	{
		g_pFilesystem->Connect( Sys_GetFactoryThis() );
		//g_pFilesystem->Init();

		char pszSearchPath[512] = { 0 };
		sprintf( pszSearchPath, "garrysmod/%s", FILESYSTEM_JAIL_PATH );

		g_pFilesystem->AddSearchPath( pszSearchPath, "GAME" );
	}

	return true;
}

bool UnloadFilesystem( )
{
	if( g_pFilesystem ) {
		// crash on reconnect???
		//g_pFilesystem->Disconnect( );
		char pszSearchPath[512] = { 0 };
		sprintf( pszSearchPath, "garrysmod/%s", FILESYSTEM_JAIL_PATH );

		g_pFilesystem->RemoveSearchPath( pszSearchPath, "GAME" );

	}
	return true;
}

}