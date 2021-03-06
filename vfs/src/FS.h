#ifndef VFS_FS_H
#define VFS_FS_H

#include <filesystem.h>

#if defined _WIN32
	#define FILESYSTEM_STEAM_DLL "filesystem_stdio.dll"
#else
	#define FILESYSTEM_STEAM_DLL "filesystem_stdio.so"
#endif

#define FILESYSTEM_JAIL_PATH "vfs"


#if defined _WIN32
	//#define WIN32_LEAN_AND_MEAN
	//#define VC_EXTRALEAN
	//#include <windows.h>
	#include <direct.h>
	#include <stdlib.h>
	#include <stdio.h>
#else
	 #include <unistd.h>
#endif


namespace FS
{
	
	enum ReadWrite
	{
		data,
		string,
		uint32,
		uint16,
		uint8,
		fp,
		doublefp,
		byte
	};

	bool LoadFilesystem( );
	bool UnloadFilesystem( );

	extern IFileSystem *g_pFilesystem;

	bool RemoveDir(const char * pathname);
	
}

#endif // VFS_FS_H