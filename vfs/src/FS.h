#ifndef VFS_FS_H
#define VFS_FS_H

#include <filesystem.h>

// what about servers???
#if defined WIN32
	#define FILESYSTEM_STEAM_DLL "filesystem_stdio.dll"
#else
	#define FILESYSTEM_STEAM_DLL "filesystem_stdio.so"
#endif

#define FILESYSTEM_JAIL_PATH "vfs"

namespace FS
{
	enum ReadWrite
	{
		ReadWrite_data,
		ReadWrite_string,
		ReadWrite_uint32,
		ReadWrite_uint16,
		ReadWrite_uint8,
		ReadWrite_float,
		ReadWrite_double,
		ReadWrite_byte
	};

	bool LoadFilesystem( );
	bool UnloadFilesystem( );

	extern IFileSystem *g_pFilesystem;
}

#endif // VFS_FS_H