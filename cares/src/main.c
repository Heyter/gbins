
#ifdef WIN32
	
	#include <stdio.h> 
	#define snprintf _snprintf 
	#define vsnprintf _vsnprintf 
	#define strcasecmp _stricmp 
	#define strncasecmp _strnicmp 
	
	#include <winsock2.h>
	
#else
	#include <dlfcn.h>
	#include <sys/mman.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CARES_STATICLIB
#include "ares.h"
#include "ares_dns.h"


ares_channel channel;
int  nfds = AF_INET;
fd_set read_fds, write_fds;
struct timeval *tvp, tv;

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#ifdef _WIN32
	#define GLUA_DLL_EXPORT  __declspec( dllexport ) 
#else
	#define GLUA_DLL_EXPORT	 __attribute__((visibility("default"))) 
#endif




lua_State* L;




static void callback(void *arg, int status, int timeouts, struct hostent *host)
{
	char **p;

	(void)timeouts;

	
	if (!L) return;
	
	lua_getglobal(L, "cares");                 		
		lua_getfield(L, -1, "Callback");         
		
		
		lua_pushstring(L,(char *) arg);
			
		lua_pushnumber(L,(double)status);
		
		int vars=2;
			
		if (status == ARES_SUCCESS)
		{
			for (p = host->h_addr_list; *p; p++)
			{
				
				char addr_buf[128] = "<INVALID>";

				inet_ntop(host->h_addrtype, *p, addr_buf, sizeof(addr_buf));

				lua_pushstring(L,addr_buf);
				lua_pushstring(L,host->h_name);

				vars+=2;
			}
		} 
			
		if (lua_pcall(L, vars, 0, 0) == 0) {
			
		} else { // errored
			const char* err = lua_tostring(L, -1);
			printf("<CARES FAIL: %s>\n",err);
		}

	lua_pop(L, 1); 
	
	return;

	
}

int __Resolve(lua_State *LL) 
{
	const char *resolveAddr = luaL_checkstring(L, 1);
	const char *optarg = luaL_checkstring(L, 2);
	
	if (!resolveAddr || !optarg) return 0;
	
	int  addr_family = AF_INET;
	struct in_addr addr4;
	struct ares_in6_addr addr6;
  

    if (!strcasecmp(optarg,"a"))
		addr_family = AF_INET;
    else if (!strcasecmp(optarg,"aaaa"))
		addr_family = AF_INET6;




    if (inet_pton(AF_INET, resolveAddr, &addr4) == 1)
    {
        ares_gethostbyaddr(channel, &addr4, sizeof(addr4), AF_INET, callback,
                            (void *) resolveAddr);
    }
    else if (inet_pton(AF_INET6, resolveAddr, &addr6) == 1)
    {
        ares_gethostbyaddr(channel, &addr6, sizeof(addr6), AF_INET6, callback,
                            (void *) resolveAddr);
    }
    else
    {
        ares_gethostbyname(channel, resolveAddr, addr_family, callback, (void *) resolveAddr);
    }


  return 0;
}

/*
LUA_FUNCTION(Die) {
	Unload(L);
	return 0;
}*/

struct timeval tv2 = {0,0};

int __Call(lua_State *LL) 
{
	L=LL;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    nfds = ares_fds(channel, &read_fds, &write_fds);
	lua_pushnumber(L,(double)nfds);
    if (nfds == 0) {
		return 1;
	}

    select(nfds, &read_fds, &write_fds, NULL, &tv2);
    ares_process(channel, &read_fds, &write_fds);


    return 1;
}



GLUA_DLL_EXPORT int gmod13_open( lua_State* LL )
{
	L=LL;
	

	int status = AF_INET;
	#ifdef USE_WINSOCK
		WORD wVersionRequested = MAKEWORD(USE_WINSOCK,USE_WINSOCK);
		WSADATA wsaData;
		WSAStartup(wVersionRequested, &wsaData);
	#endif
	status = ares_library_init(ARES_LIB_INIT_ALL);
	
	if (status != ARES_SUCCESS) {
		printf("ares_library_init: %s\n", ares_strerror(status));
		return 0;
	}


	status = ares_init(&channel);
	if (status != ARES_SUCCESS)
	{
		printf("ares_init: %s\n", ares_strerror(status));
		return 0;
	}

	lua_getglobal(L, "cares");
	lua_pushcfunction(L, __Resolve);
	lua_setfield(L, -2, "__Resolve");
	lua_pushcfunction(L, __Call);
	lua_setfield(L, -2, "__Call");
	
	return 0;
}

GLUA_DLL_EXPORT int gmod13_close( lua_State* LL )
{
	 ares_destroy(channel);

	 ares_library_cleanup();

	#ifdef USE_WINSOCK
	  WSACleanup();
	#endif
	
	L=NULL;
	
	return 0;
}



