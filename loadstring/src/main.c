#ifdef _WIN32
	#define GLUA_DLL_EXPORT  __declspec( dllexport ) 
#else
	

	#include <dlfcn.h>

	#define LUA_DL_DLOPEN 1
	#define GLUA_DLL_EXPORT	 __attribute__((visibility("default"))) 
#endif


#include <stdlib.h>
#include <string.h>

#define loadlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"



static int load_aux (lua_State *L, int status) {
  if (status == 0)  /* OK? */
    return 1;
  else {
    lua_pushnil(L);
    lua_insert(L, -2);  /* put before error message */
    return 2;  /* return nil plus error message */
  }
}

static int luaB_loadstringcustom (lua_State *L) {
  size_t l;
  const char *s = luaL_checklstring(L, 1, &l);
  const char *chunkname = luaL_optstring(L, 2, s);
  return load_aux(L, luaL_loadbuffer(L, s, l, chunkname));
}

LUALIB_API int luaopen_loadstring (lua_State *L) {
  lua_pushcfunction(L, luaB_loadstringcustom);
  lua_setglobal(L, "loadstring");
  return 0;
}




GLUA_DLL_EXPORT int gmod13_open( lua_State* L )
{
	dlopen("garrysmod/bin/lua_shared_srv.so",RTLD_GLOBAL);
	return luaopen_loadstring(L);
}

GLUA_DLL_EXPORT int gmod13_close( lua_State* L )
{
	return 0;
}



