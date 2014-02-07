//#include "GMLuaModule.h"
extern "C" {
	#include "lua.h"
	int luaopen_socket_core(lua_State *L);
	int luaopen_mime_core(lua_State *L);
}


extern "C" __attribute__( ( visibility("default") ) ) int gmod13_open( lua_State* L )
{
	/* Ugly hack: */
	lua_newtable(L);
	lua_pushcfunction(L, luaopen_socket_core);
	lua_setfield(L, -2, "luaopen_socket_core");
	lua_pushcfunction(L, luaopen_mime_core);
	lua_setfield(L, -2, "luaopen_mime_core");
	lua_setglobal(L, "luasocket_stuff"); /* ugly hack end */

	return 0;
}

extern "C" __attribute__( ( visibility("default") ) ) int gmod13_close( lua_State* L )
{
	return 0;
}



