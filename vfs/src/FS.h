#if !defined(VFS_FS_H)
#define VFS_FS_H

#if defined(_MSC_VER)
#pragma once
#endif

#include "filesystem.h"

// what about servers???
#if defined(WIN32)
	#define FILESYSTEM_STEAM_DLL	"filesystem_stdio.dll"
#else
	#define FILESYSTEM_STEAM_DLL	"filesystem_stdio.so"
#endif
#define FILESYSTEM_JAIL_PATH	"vfs"

#define VFS_READWRITE_DATA		0
#define VFS_READWRITE_STRING	1
#define VFS_READWRITE_UINT32	2
#define VFS_READWRITE_UINT16	3
#define VFS_READWRITE_UINT8		4
#define VFS_READWRITE_FLOAT		5
#define VFS_READWRITE_DOUBLE	6
#define VFS_READWRITE_BYTE		7

namespace FS {

bool LoadFilesystem(void);
bool UnloadFilesystem(void);

extern IFileSystem* g_pFilesystem;

}

#endif // VFS_FS_H