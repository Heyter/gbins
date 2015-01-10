#include "StdAfx.h"

#include "gm_lua.h"

#define DECLARE_FUNCS(x) x##_t g##x = NULL;

FUNCLIST(DECLARE_FUNCS)
lua_resume_t glua_resume = NULL;

struct FunctionSig {
	const char* sig;
	const char* mask;
	void* pointer;
};



#if defined __WIN32__
	#define MODULE_T HMODULE
	#define OPEN_LIBRARY(x) LoadLibrary("garrysmod/bin/" x ".dll")
	#define CLOSE_LIBRARY FreeLibrary
	#define GETSYMBOL GetProcAddress
#elif defined __LINUX__
	#define MODULE_T void*
	#define OPEN_LIBRARY(x) dlopen( "garrysmod/bin/" x "_srv.so", RTLD_LAZY)
	#define CLOSE_LIBRARY dlclose
	#define GETSYMBOL dlsym

#elif defined __MACOSX__
	#define MODULE_T void*
	#define OPEN_LIBRARY(x) dlopen( ##x ".so", RTLD_FIRST)
	#define CLOSE_LIBRARY dlclose
	#define GETSYMBOL dlsym
#endif

#define GETFUNCS(x) g##x = (x##_t) GETSYMBOL(lua_shared, #x);

MODULE_T lua_shared = NULL;



void InitLuaAPI() {
	printf("Replacing luajit...");
	lua_shared = OPEN_LIBRARY( "lua_shared" );
	
	FUNCLIST(GETFUNCS)
	glua_resume = (lua_resume_t) GETSYMBOL(lua_shared, "lua_resume_real");
	printf("%p %p\n",lua_shared,glua_resume);
}

void ShutdownLuaAPI() {
	CLOSE_LIBRARY(lua_shared);
}

#define DEBUG_FUNCS(x) if (g##x) printf("Found %s at %p\n", #x, g##x); else printf("Failed to find %s.\n", #x);

void DebugLuaAPI() {
	FUNCLIST(DEBUG_FUNCS)
}