
#include <string.h>
#include <stdio.h>

#include <dlfcn.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "memutils.h"


#include "detours.h"

//#include "vfnhook.h"



namespace ghostinj {

	typedef bool (*thook1) (  );

	thook1 original_hook1 = NULL;
	MologieDetours::Detour<thook1>* detour_hook1 = NULL;


	bool hook_hook1(  )
	{
		printf("[ghostinj] Prevented creating rcon server\n");
		return false;
	}



	// Called when the library is loaded and before dlopen() returns

	void *lHandle;

}

void __attribute__ ((constructor)) gh_load(void);
void __attribute__ ((destructor)) gh_unload(void);
void gh_load(void)
{
	using namespace ghostinj;
	
	const char * hook1_sym = "_ZN11CRConServer12CreateSocketEv";
	lHandle=NULL;
	lHandle = dlopen( "engine_srv.so", RTLD_LAZY  );
	if ( lHandle )
	{

			original_hook1 = (thook1) ResolveSymbol( lHandle, hook1_sym );
			if (original_hook1) {
					try {
							detour_hook1 = new MologieDetours::Detour<thook1>(original_hook1, hook_hook1);
					}
					catch(MologieDetours::DetourException &e) {
							printf("hook1: Detour failed: Internal error?\n");
							return;
					}
			} else {
					printf("hook1: Detour failed: Signature not found. (plugin needs updating)\n");
					return;
			}

	} else {
			printf("hook1: Detour failed. File changed?\n");
	}

}

// Called when the library is unloaded and before dlclose()
// returns
void gh_unload(void)
{
	using namespace ghostinj;
	delete detour_hook1;
	if (lHandle) dlclose( lHandle );
}
