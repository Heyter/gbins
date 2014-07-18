#ifndef VFS_FS_H
#define VFS_FS_H

#include <filesystem.h>

#if defined _WIN32
	#define FILESYSTEM_STEAM_DLL "filesystem_stdio.dll"
#else
	#define FILESYSTEM_STEAM_DLL "filesystem_stdio.so"
#endif

#define FILESYSTEM_JAIL_PATH "vfs"

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
}

#endif // VFS_FS_H