#include "hookcall.h"

//int Lhook::toppos_start2 = 0;
//int Lhook::toppos_start = 0;
//lua_State* Lhook::state = NULL;

Lhook::Lhook(const char * name, lua_State* _state ) 
{
	using namespace GarrysMod::Lua;
	state = _state;
	
	//Warning("hook.Run %s\n",name);
	
	toppos_start = LUA->Top();
	
		LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB ); // Push ( 1 )
		LUA->GetField( -1, "hook" );                      // Push ( 1 )
		LUA->GetField( -1, "Run" );                       // Push ( 1 )
		
		LUA->PushString( name );   
	//#ifdef DEBUG 
	//	Warning("HOOK: %s\n",name);
	//#endif
	
	toppos_start2 = LUA->Top();
	
};

Lhook::~Lhook( void ) 
{
	Lhook::end();
}

void Lhook::end()
{
	using namespace GarrysMod::Lua;
	int current_top = LUA->Top();
	int pop_count = current_top-toppos_start;
	//Warning("hookEnd %d\n", pop_count );
	for (int i = 0; i < pop_count; ++i)
		LUA->Pop();

	if (LUA->Top()!=toppos_start) {
		Warning("TOP IS WRONG\n");
	}
	
};

bool Lhook::call() {
	using namespace GarrysMod::Lua;
	bool ret = Lhook::call(0);
	return ret;
};

bool Lhook::call(unsigned int rets) {
	
	//Warning("hookCall %d\n",rets);
	
	
	using namespace GarrysMod::Lua;
	
	int top_pos = LUA->Top();
	int topcount = top_pos-toppos_start2;
	
	if( LUA->PCall( 1+topcount, rets, 0 ) != 0 ) 
	{
		Warning( "[hook.Call ERROR] %s\n", LUA->GetString( ) ); 
		return false;
	}
	
	return true;
};

