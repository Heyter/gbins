#include "StdAfx.h"

#include <lua.hpp>
extern "C" {

// Including lj_obj.h causes errors
	#include "lj_obj.h"
}
#include "gm_lua.h"

#ifdef __WIN32__
	#include <windows.h>
	#include "sigscan/sigscan.h"
	#include "detours/detours.h"
	#define DETOUR_ATTACH_FUNCS(x) if (g##x) DetourAttach(&(PVOID&)g##x, x);
	#define DETOUR_DETACH_FUNCS(x) if (g##x) DetourDetach(&(PVOID&)g##x, x);
#else
	#undef min
	#undef max
	#include "detours.h"
	#include "memutils.h"
	#include "vfnhook.h"
	#include <dlfcn.h>
#endif


lua_newthread_t lua_newthread_p = NULL;
luaL_openlibs_t luaL_openlibs_p = NULL;

int (*luaL_newmetatable_type_p)(lua_State*, const char*, int);
extern "C" {
	int (*lua_resume_real)(lua_State*,int narg);
	LUA_API lua_State *(lua_newthread) (lua_State *L);
}



MologieDetours::Detour<lua_newthread_t>* lua_newthread_mologie_hook = NULL;

lua_State *lua_newthread_hook(lua_State* L) {
	lua_State* L1 = lua_newthread_mologie_hook->GetOriginalFunction()(L);
	printf("newthread\n");
	L1->LuaInterface = L->LuaInterface;
	return L1;
}

int luaL_newmetatable_type(lua_State* L, const char* tname, int typenum) {
	int ret = luaL_newmetatable(L, tname);
    lua_pushstring(L, tname);
    lua_setfield(L, -2, "MetaName");
	lua_pushinteger(L, typenum);
	lua_setfield(L, -2, "MetaID");
	return ret;
}

static const luaL_Reg lj_lib_load[] = {
  { "",			luaopen_base },
  { LUA_LOADLIBNAME,	luaopen_package },
  { LUA_TABLIBNAME,	luaopen_table },
  { LUA_OSLIBNAME,	luaopen_os },
  { LUA_OSLIBNAME,	luaopen_io },
  { LUA_STRLIBNAME,	luaopen_string },
  { LUA_MATHLIBNAME,	luaopen_math },
  { LUA_DBLIBNAME,	luaopen_debug },
  { LUA_BITLIBNAME,	luaopen_bit },
  { LUA_JITLIBNAME,	luaopen_jit },
  { NULL,		NULL }
};

static const luaL_Reg lj_lib_preload[] = {
  { LUA_FFILIBNAME,	luaopen_ffi },
  { NULL,		NULL }
};

#define lua_erasefield(L, n) lua_pushnil(L); lua_setfield(L, -2, n)

void luaL_openlibs(lua_State* L) {
	printf("openlibs\n");
	const luaL_Reg *lib;
	for (lib = lj_lib_load; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_pushstring(L, lib->name);
		lua_call(L, 1, 0);
	}
	luaL_findtable(L, LUA_REGISTRYINDEX, "_PRELOAD", sizeof(lj_lib_preload)/sizeof(lj_lib_preload[0])-1);
	for (lib = lj_lib_preload; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_setfield(L, -2, lib->name);
	}
	lua_pop(L, 1);

	lua_getfield(L, LUA_GLOBALSINDEX, "os");
	lua_erasefield(L, "execute");
	lua_erasefield(L, "exit");
	lua_erasefield(L, "getenv");
	lua_erasefield(L, "remove");
	lua_erasefield(L, "rename");
	lua_erasefield(L, "tmpname");
	lua_pop(L, 1);
	
	lua_getglobal(L,"require");
	lua_setglobal(L,"require_real");
}

struct ILua {
	void* vtable;
	lua_State* L;
};

void Init() {
	InitLuaAPI();
#ifdef _DEBUG
	DebugLuaAPI();
#endif
#ifdef __WIN32__
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	FUNCLIST(DETOUR_ATTACH_FUNCS)
	if (glua_resume) DetourAttach(&(PVOID&)glua_resume, lua_resume);
	if (lua_newthread_p) DetourAttach(&(PVOID&)lua_newthread_p, lua_newthread_hook);
	if (luaL_openlibs_p) DetourAttach(&(PVOID&)luaL_openlibs_p, luaL_openlibs_hook);
	DetourTransactionCommit();
#else
	lua_resume_real = lua_resume;
	printf("DGB: %p %p\n",lua_newthread,lua_newthread_hook);
	lua_newthread_p = lua_newthread;
	try {
		lua_newthread_mologie_hook = new MologieDetours::Detour<lua_newthread_t>(lua_newthread, lua_newthread_hook);
	}
	catch(MologieDetours::DetourException &e) {
		printf("hook1: Detour failed: Internal error?\n");
		return;
	}

	
	//FUNCLIST(DETOUR_ATTACH_FUNCS)
	//if (glua_resume) DetourAttach(&(PVOID&)glua_resume, lua_resume);
	//if (lua_newthread_p) DetourAttach(&(PVOID&)lua_newthread_p, lua_newthread_hook);
	//if (luaL_openlibs_p) DetourAttach(&(PVOID&)luaL_openlibs_p, luaL_openlibs_hook);
#endif
}

void Shutdown() {
#ifdef __WIN32__
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	FUNCLIST(DETOUR_DETACH_FUNCS)
	if (glua_resume) DetourDetach(&(PVOID&)glua_resume, lua_resume);
	if (lua_newthread_p) DetourDetach(&(PVOID&)lua_newthread_p, lua_newthread_hook);
	if (luaL_openlibs_p) DetourDetach(&(PVOID&)luaL_openlibs_p, luaL_openlibs_hook);
	DetourTransactionCommit();
#endif

#ifdef __LINUX__
	//FUNCLIST(DETOUR_DETACH_FUNCS)
	//if (glua_resume) DetourDetach(&(PVOID&)glua_resume, lua_resume);
	//if (lua_newthread_p) DetourDetach(&(PVOID&)lua_newthread_p, lua_newthread_hook);
	//if (luaL_openlibs_p) DetourDetach(&(PVOID&)luaL_openlibs_p, luaL_openlibs_hook);
#endif
	ShutdownLuaAPI();
}