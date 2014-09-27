#include <string.h>
#include <stdio.h>

#include <dlfcn.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "memutils.h"

#include "vfnhook.h"

// MUHA
#include "bloom/bloom_filter.hpp"


#include "detours.h"
#include "tier1/netadr.h"
#include "tier0/dbg.h"


extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
}

void LOG(const char * x) {
	int ret = write(2,x,strlen(x));
	(void)(ret);
}

typedef bool (*	tShouldDiscard ) ( const netadr_t&  ) ;
tShouldDiscard original_ShouldDiscard = NULL;

lua_State* L = NULL;

MologieDetours::Detour<tShouldDiscard>* detour_ShouldDiscard = NULL;

typedef void (*	tFilter_SendBan ) ( const netadr_t& ) ;
tFilter_SendBan original_Filter_SendBan = NULL;
MologieDetours::Detour<tFilter_SendBan>* detour_Filter_SendBan = NULL;

/*
bool hook_ShouldDiscard( const netadr_t& adr )
{
	static bool lastret = false;
	static unsigned lastip = 0;
	
	if (!L) return false;
	bool ret = false;
	
	lua_getglobal(L, "Filter_ShouldDiscard");                 		
		
		unsigned in = *(unsigned *)&adr.ip[0];
		unsigned left_in = in >> 16; //( in & 0xFFFF0000 ) >> 16;
		unsigned right_in = in&0x0000FFFF;
		
		lua_pushnumber(L,left_in);
		lua_pushnumber(L,right_in);
		
		if (lua_pcall(L, 2, 1, 0) == 0) {

			if (lua_isboolean(L, -1)) {
				ret = lua_toboolean(L, -1);
				lua_pop(L, 1);
				return ret;
			}
			
		} else {
			LOG("<FAIL: ");
			const char* err = lua_tostring(L, -1);
			LOG(err);
			lua_pop(L, 1); 
			LOG(">\n");
		}

	lua_pop(L, 1); 
	
	return ret;
}
*/

bloom_filter* bloom_filt = NULL;

bool hook_ShouldDiscard( const netadr_t& adr )
{
	if (!bloom_filt) return false;
	
	unsigned in = *(unsigned *)&adr.ip[0];
		
	if (bloom_filt->contains(in))
	{
		return false;
	}
	return true;
}



void hook_Filter_SendBan( const netadr_t& adr )
{
}


int EnableFirewallWhitelist( lua_State* L )
{
	if (!original_ShouldDiscard || !bloom_filt) return 0;
	bool hook = lua_toboolean(L,1);
	lua_pop(L, 1);
	if (hook && !detour_ShouldDiscard) 
	{
		try {
			detour_ShouldDiscard = new MologieDetours::Detour<tShouldDiscard>(original_ShouldDiscard, hook_ShouldDiscard);
		}
		catch(MologieDetours::DetourException &e) {
			LOG("ShouldDiscard: Detour failed: Internal error?\n");
			return 0;
		}
	}
	else if (!hook && detour_ShouldDiscard) {
		delete detour_ShouldDiscard;
		detour_ShouldDiscard=NULL;
	}
	
	lua_pushboolean(L,true);
	return 1;
}




int Bloom_WhitelistIP( lua_State* L )
{
	if (!bloom_filt || !lua_isnumber(L,1)) {
		return 0;
	}
	
	double ip = lua_tonumber(L,1);
	
	unsigned in = (unsigned)ip;
	
	bloom_filt->insert(in);

	//lua_pop(L, 1);
	return 1;
}


int Bloom_WhitelistReset( lua_State* L )
{

	if (bloom_filt) {
		delete bloom_filt;
	}
	
	bloom_parameters parameters;

	parameters.projected_element_count = 200;

	parameters.false_positive_probability = 0.0001; 

	parameters.random_seed = 0xA57AC3B2;

	if (!parameters)
	{
		return 0;
	}

	parameters.compute_optimal_parameters();
	bloom_filt = new bloom_filter(parameters);

	lua_pushboolean(L,true);
	return 1;
}


extern "C" __attribute__( ( visibility("default") ) ) int gmod13_open( lua_State* LL )
{
	void *lHandle = dlopen( "bin/engine_srv.so", RTLD_LAZY  );

	L=LL;

	if ( lHandle )
	{
		original_ShouldDiscard = (tShouldDiscard)ResolveSymbol( lHandle, "_Z20Filter_ShouldDiscardRK8netadr_s" );
		if (original_ShouldDiscard) {
			lua_pushcfunction(L,EnableFirewallWhitelist);
			lua_setglobal(L, "EnableFirewallWhitelist");
			
			lua_pushcfunction(L,Bloom_WhitelistIP);
			lua_setglobal(L, "WhitelistIP");

			lua_pushcfunction(L,Bloom_WhitelistReset);
			lua_setglobal(L, "WhitelistReset");
			
		} else {
			LOG("ShouldDiscard: Detour failed: Signature not found. (plugin needs updating)\n");
		}
 
		original_Filter_SendBan = (tFilter_SendBan)ResolveSymbol( lHandle, "_Z14Filter_SendBanRK8netadr_s" );
		if (original_Filter_SendBan) {
			try {
				detour_Filter_SendBan = new MologieDetours::Detour<tFilter_SendBan>(original_Filter_SendBan, hook_Filter_SendBan);
			}
			catch(MologieDetours::DetourException &e) {
				LOG("Filter_SendBan: Detour failed: Internal error?\n");
			}
		} else {
			LOG("Filter_SendBan: Detour failed: Signature not found. (plugin needs updating)\n");
		}

		dlclose( lHandle );
	} else 
	{
		LOG("handle failed???\n");
	}
	
	return 0;
}

extern "C" __attribute__( ( visibility("default") ) ) int gmod13_close( lua_State* LL )
{
	if (detour_ShouldDiscard) 
		delete detour_ShouldDiscard;

	detour_ShouldDiscard = NULL;

	if (detour_Filter_SendBan) 
		delete detour_Filter_SendBan;
	L = NULL;
	return 0;
}