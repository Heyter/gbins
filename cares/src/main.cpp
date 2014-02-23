
#ifdef WIN32
	
	#include <stdio.h> 
	#pragma comment(lib, "libcares")
	#define snprintf _snprintf 
	#define vsnprintf _vsnprintf 
	#define strcasecmp _stricmp 
	#define strncasecmp _strnicmp 
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

//#include "common/GMLuaModule.h"
#include "ILuaModuleManager.h"
GMOD_MODULE( Load, Unload );

ILuaInterface* gLua;

static void callback(void *arg, int status, int timeouts, struct hostent *host)
{
	char **p;

	(void)timeouts;

	
	ILuaObject *hookTable = gLua->GetGlobal( "cares" );
	ILuaObject *hookCallFunc = hookTable->GetMember( "Callback" );
	
	gLua->Push( hookCallFunc );

	gLua->Push((char *) arg);
		
	gLua->Push((double)status);
	
	int vars=2;	
		
	if (status == ARES_SUCCESS)
	{
		for (p = host->h_addr_list; *p; p++)
		{
			
			char addr_buf[46] = "HUGE ERROR, wTF!";

			inet_ntop(host->h_addrtype, *p, addr_buf, sizeof(addr_buf));
			//gLua->Msg("[Ares DLL] Resolv:  %-32s\t%s\n", host->h_name, addr_buf);

			gLua->Push(addr_buf);
			gLua->Push(host->h_name);

			vars+=2;
		}
	}

		
	gLua->Call(vars);
	
}

LUA_FUNCTION(Resolve) {
	
	int  addr_family = AF_INET;
	struct in_addr addr4;
	struct ares_in6_addr addr6;
  
	gLua->CheckType( 1, Type::STRING );
	const char *resolveAddr = gLua->GetString( 1 );
	
	gLua->CheckType( 2, Type::STRING );
	const char *optarg = gLua->GetString( 2 );


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

timeval tv2 = {0,0};

LUA_FUNCTION(Call) {
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    nfds = ares_fds(channel, &read_fds, &write_fds);
	Lua()->Push((double)nfds);
    if (nfds == 0) {
		return 1;
	}

    tvp = ares_timeout(channel, &tv2, &tv);
    select(nfds, &read_fds, &write_fds, NULL, &tv2);
    ares_process(channel, &read_fds, &write_fds);


    return 1;
}

int Load( lua_State *L )
{
	gLua = Lua();

	// Initialize.
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

	ILuaObject *cares = Lua()->GetGlobal( "cares" );
		cares->SetMember( "__Resolve", Resolve );
		cares->SetMember( "__Call", Call );
		
	cares->UnReference();

	return 0;
}

int Unload( lua_State *L )
{
	 ares_destroy(channel);

	 ares_library_cleanup();

	#ifdef USE_WINSOCK
	  WSACleanup();
	#endif

	return 0;
}



