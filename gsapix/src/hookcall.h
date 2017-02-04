#ifndef LHOOK_LOADED
#define LHOOK_LOADED

#include <GarrysMod/Lua/Interface.h>
#include "dbg.h"

class Lhook {
	
	public:
	
		Lhook(const char * name,lua_State* state);
		~Lhook( void );
		
		bool call(unsigned int numargs,unsigned int rets);
		bool call(unsigned int rets);
		bool call();
		void end();
		
		int toppos_start2;
		int toppos_start;
		lua_State* state;
		
};



 



#endif