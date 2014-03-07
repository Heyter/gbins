#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GarrysMod/Lua/Interface.h>
 
using namespace GarrysMod::Lua;
 
static int title_SetConsoleTitle(lua_State* state)
{
	if(!LUA->IsType(1, Type::STRING)) return 0;

 	const char * TERMT = getenv("TERM");
	bool is_screen = strcmp(TERMT?TERMT:"", "screen") == 0;

	// overridable
	if(LUA->IsType(2, Type::BOOL)) {
		is_screen = LUA->GetBool(2);
	}
 
	if(is_screen) {
		printf("\x1BP\x1B]0;%s\x07\x1B\\", LUA->GetString(1));
	} else {
		printf("\x1B]0;%s\x07", LUA->GetString(1));
	}
 
	fflush(stdout);
 
	return 0;
}
 
GMOD_MODULE_OPEN()
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->PushString("SetConsoleTitle");
	LUA->PushCFunction(title_SetConsoleTitle);
	LUA->SetTable(-3);
 
	return 0;
}
 
GMOD_MODULE_CLOSE()
{
	return 0;
}
