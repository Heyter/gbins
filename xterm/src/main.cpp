#include <tier0/dbg.h>

#include <stdio.h>

#include <GarrysMod/Lua/Interface.h>

#ifdef KILL_INPUT
	#include <dlfcn.h>
	#include <pthread.h>
	#include "memutils.h"
#endif


extern "C" {
	#include "rgb2xterm.h"
}
#include <Color.h>



SpewOutputFunc_t oldspew = NULL;

// lua_run Msg"Msg!\n" MsgN"MsgN" ErrorNoHalt"ErrorNoHalt\n" print"print"  MsgC(Color(111,255,111),"MsgC!\n") error"error"

SpewRetval_t newspew( SpewType_t Type, const char *msg )
{
	
	SpewRetval_t ret;

	if ( !msg || *msg == '\0')
		return SPEW_CONTINUE;
		
	const Color*  c = GetSpewOutputColor( );
	int RED    = 0;
	int GREEN  = 0;
	int BLUE   = 0;
	int ALPHA  = 0;
	c->GetColor(RED,GREEN,BLUE,ALPHA);
	bool printcol = RED>0 || GREEN>0 || BLUE>0;
	bool white = RED==255 && GREEN == 255 && BLUE == 255;
	if (Type==SPEW_WARNING && white) {
		RED=255;
		GREEN=100;
		BLUE=50;
	} else if (Type==SPEW_LOG && white) {
		RED=200;
		GREEN=235;
		BLUE=255;
	} else if ( (Type==SPEW_ERROR || Type==SPEW_ASSERT) && white) {
		RED=255;
		GREEN=0;
		BLUE=0;
	} else if (RED==156 && GREEN==241 && BLUE==255) { // Msg
		RED=255;
		//GREEN=241;
		BLUE=156;
	} else if (RED==136 && GREEN==221 && BLUE==221) { // MsgN
		RED=221;
		//GREEN=221;
		BLUE=136;
	} else if (RED==136 && GREEN==221 && BLUE==255) { // error
		RED=255;
		GREEN=221 - 120;
		BLUE=136 - 80;
	}

	if (!printcol) {
		if (Type != SPEW_MESSAGE) {
			RED = 255;
			BLUE = 255;
		} else {
			return SPEW_CONTINUE;
		}
	}
	
	int val = rgb2xterm(RED,GREEN,BLUE);
	const char * grp = GetSpewOutputGroup();
	
	//printf("<%d,%d,%d,%d|%d,%d,%s>\x1b[38;5;%dm",RED,GREEN,BLUE,ALPHA,  Type,GetSpewOutputLevel(),grp?grp:"", val); // set color
	
	char buf[20] = "";
	snprintf(buf,sizeof(buf),"\x1b[38;5;%dm",val); // set color
	
	oldspew( Type, buf );
	
	oldspew( Type, msg );
	
	oldspew( Type, "\x1b[0m" );

	return SPEW_CONTINUE; // Anything but SPEW_CONTINUE only closes the server.
}


GMOD_MODULE_OPEN( )
{
	oldspew = GetSpewOutputFunc();
	
	// TEST: Get original func and remove the weird new buffering
	//SpewOutputFunc( NULL );
	//
	//SpewOutputFunc_t origspew = GetSpewOutputFunc();
	//
	//if (! (oldspew==origspew) && origspew!=NULL) {
	//	fputs("\nWARN: oldspew!=origspew. Overriding!!!\n",stderr);
	//	printf("    >> orig %p cur %p new %p\n",origspew,oldspew, newspew);
	//	oldspew = origspew;
	//}
	// UNDONE: Embrace new!
	
	
	SpewOutputFunc( newspew );
	
	
#ifdef KILL_INPUT
	void * f = dlopen ("dedicated_srv.so", RTLD_NOW);
	if (f) {
		pthread_t * g_threadid = (pthread_t *) ResolveSymbol (f, "_ZL10g_threadid");
		if (g_threadid!=NULL) {
						
			printf("g_threadid: %p/%p. Assigning NULL.\n",g_threadid,*g_threadid);
			*g_threadid = -1;
			g_threadid = -1;
			
			
		} else fputs("can't resolve g_threadid from dedicated_srv\n",stderr);
	} else fputs("cant open dedicated_srv\n",stderr);
#endif

	return 0;
}

GMOD_MODULE_CLOSE( )
{
	SpewOutputFunc( oldspew );
	return 0;
}

