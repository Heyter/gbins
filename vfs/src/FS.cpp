#include "FS.h"

#if defined(WIN32)
	#include <Windows.h>
#endif

namespace FS {

IFileSystem* g_pFilesystem = NULL;

void* CreateInterface(const char *pName, int *pReturnCode)
{
	if( pReturnCode )
		*pReturnCode = IFACE_FAILED;

	return NULL;
}

bool LoadFilesystem(void)
{
#if defined(WIN32)

	HMODULE hModule = GetModuleHandle(FILESYSTEM_STEAM_DLL);
	if( !hModule )
	{
		MessageBox(NULL, "Unable to locate handle of 'filesystem_steam.dll'", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	CreateInterfaceFn _CreateInterface = (CreateInterfaceFn)GetProcAddress(hModule, "CreateInterface");
	if( !_CreateInterface )
	{
		MessageBox(NULL, "Unable to locate export 'CreateInterface' in filesystem_steam", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	int nReturnCode = 0;
	g_pFilesystem = (IFileSystem*)_CreateInterface(FILESYSTEM_INTERFACE_VERSION, &nReturnCode);

#else

	// TODO: Linux support, sigh...

#endif

	if( g_pFilesystem )
	{
		g_pFilesystem->Connect(CreateInterface);

		// TODO: Figure out if do we need to call Init?
		// apparently it works without, so leave it alone.
		// g_pFilesystem->Init();

		char pszSearchPath[512] = {0};
		sprintf(pszSearchPath, "garrysmod/%s", FILESYSTEM_JAIL_PATH);

		g_pFilesystem->AddSearchPath(pszSearchPath, "GAME");
	}

	return true;
}

bool UnloadFilesystem(void)
{
	if( g_pFilesystem )
		g_pFilesystem->Disconnect();

	return true;
}


}