#include "FS.h"

namespace FS
{

	IFileSystem *g_pFilesystem = NULL;

	bool LoadFilesystem( )
	{
		
		CSysModule* FileSystemFactoryDLL = NULL;
		if (!Sys_LoadInterface(FILESYSTEM_STEAM_DLL, FILESYSTEM_INTERFACE_VERSION, &FileSystemFactoryDLL, (void**)&g_pFilesystem)) 
			return false;
		
		if( g_pFilesystem == nullptr ) return false;

		g_pFilesystem->AddSearchPath( "garrysmod/" FILESYSTEM_JAIL_PATH, "GAME" );
		
		return true;
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


	bool RemoveDir(const char * pathname)
	{
		#if defined _WIN32
			return _rmdir(pathname) == 0; 
		#else
			return rmdir(pathname)  == 0; 
		#endif
	}

}