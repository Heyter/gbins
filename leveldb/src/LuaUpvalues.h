#pragma once

#define lua_upvalueindex(i) (LUA_GLOBALSINDEX-(i))
#define LUA_GLOBALSINDEX -10002

/*
5:35 PM - ;Meepen™: you can do LUA->Push(LUA_GLOBALSINDEX - upvaluenumber)
5:35 PM - ;Meepen™: and it will be the upvalue's value
*/