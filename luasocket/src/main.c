#include "lua.h"
int luaopen_socket_core(lua_State *L);
int luaopen_mime_core(lua_State *L);

#ifdef _WIN32
	#define GLUA_DLL_EXPORT  __declspec( dllexport ) 
#else
	#define GLUA_DLL_EXPORT	 __attribute__((visibility("default"))) 
#endif

int luaopen_dummy_func(lua_State *L)  {
	return 0;
}

GLUA_DLL_EXPORT int gmod13_open( lua_State* L )
{
	lua_newtable(L);
	lua_pushcfunction(L, luaopen_dummy_func);
	lua_setfield(L, -2, "luaopen_socket_core");
	lua_pushcfunction(L, luaopen_dummy_func);
	lua_setfield(L, -2, "luaopen_mime_core");
	lua_setglobal(L, "luasocket_stuff");
	
	// let's just load it directly?
	luaopen_socket_core(L);
	luaopen_mime_core(L);
	
	return 0;
}

GLUA_DLL_EXPORT int gmod13_close( lua_State* L )
{
	return 0;
}



