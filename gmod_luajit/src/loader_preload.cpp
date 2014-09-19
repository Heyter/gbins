#ifdef PRELOADPLUGIN

#include "StdAfx.h"

#include "luareplace.h"

void __attribute__ ((constructor)) luajitpreload(void);
void __attribute__ ((destructor)) luajitpreload_unload(void);

// Called when the library is loaded and before dlopen() returns
void luajitpreload(void)
{
	Init();
}

// Called when the library is unloaded and before dlclose()
// returns
void luajitpreload_unload(void)
{
	Shutdown();
}

#endif