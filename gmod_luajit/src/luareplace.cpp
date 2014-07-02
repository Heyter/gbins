#include "StdAfx.h"

#include <lua.hpp>
extern "C" {

// Including lj_obj.h causes errors
// _BitScanForward and _BitScanReverse aren't defined in EXACTLY
// the same way somewhere else
// Work around it
struct lua_State {
  int nextgc;
  char marked;
  char gct;
  char dummy_ffid;	/* Fake FF_C for curr_funcisL() on dummy frames. */
  char status;	/* Thread status. */
  int glref;		/* Link to global state. */
  int gclist;		/* GC chain. */
  long long *base;		/* Base of currently executing function. */
  long long *top;		/* First free slot in the stack. */
  int maxstack;	/* Last free slot in the stack. */
  int stack;		/* Stack base. */
  int openupval;	/* List of open upvalues in the stack. */
  int env;		/* Thread environment (table of globals). */
  void *cframe;		/* End of C stack frame chain. */
  int stacksize;	/* True stack size (incl. LJ_STACK_EXTRA). */
  int padding[6];
  void* LuaInterface;
};

}
#include "gm_lua.h"
#include <windows.h>
#include "sigscan/sigscan.h"
#include "detours/detours.h"

#define DETOUR_ATTACH_FUNCS(x) if (g##x) DetourAttach(&(PVOID&)g##x, x);
#define DETOUR_DETACH_FUNCS(x) if (g##x) DetourDetach(&(PVOID&)g##x, x);

lua_newthread_t lua_newthread_p = lua_newthread;
luaL_openlibs_t luaL_openlibs_p = luaL_openlibs;

int (*luaL_newmetatable_type_p)(lua_State*, const char*, int);

lua_State *lua_newthread_hook(lua_State* L) {
	lua_State* L1 = lua_newthread_p(L);
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
  { LUA_STRLIBNAME,	luaopen_string },
  { LUA_MATHLIBNAME,	luaopen_math },
  { LUA_DBLIBNAME,	luaopen_debug },
  { LUA_BITLIBNAME,	luaopen_bit },
  { LUA_JITLIBNAME,	luaopen_jit },
  { NULL,		NULL }
};

static const luaL_Reg lj_lib_preload[] = {
  { NULL,		NULL }
};

#define lua_erasefield(L, n) lua_pushnil(L); lua_setfield(L, -2, n)

void luaL_openlibs_hook(lua_State* L) {
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
	lua_erasefield(L, "setlocale");
	lua_erasefield(L, "tmpname");
	lua_pop(L, 1);
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
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	FUNCLIST(DETOUR_ATTACH_FUNCS)
	if (glua_resume) DetourAttach(&(PVOID&)glua_resume, lua_resume);
	if (lua_newthread_p) DetourAttach(&(PVOID&)lua_newthread_p, lua_newthread_hook);
	if (luaL_openlibs_p) DetourAttach(&(PVOID&)luaL_openlibs_p, luaL_openlibs_hook);
	DetourTransactionCommit();
}

void Shutdown() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	FUNCLIST(DETOUR_DETACH_FUNCS)
	if (glua_resume) DetourDetach(&(PVOID&)glua_resume, lua_resume);
	if (lua_newthread_p) DetourDetach(&(PVOID&)lua_newthread_p, lua_newthread_hook);
	if (luaL_openlibs_p) DetourDetach(&(PVOID&)luaL_openlibs_p, luaL_openlibs_hook);
	DetourTransactionCommit();
	ShutdownLuaAPI();
}