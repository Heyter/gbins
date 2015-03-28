#pragma fenv_access (on)

#ifdef _WIN32
	#include <Windows.h>
	#include <float.h>

#else
	#include <pthread.h>
	#include <fpu_control.h>
#endif

#include <tier0/dbg.h>
#include <Color.h>
#include <cstdio>
#include <GarrysMod/Lua/Interface.h>


GMOD_MODULE_OPEN( )
{

    double a = 0.1;
    float f = 0.1;
    unsigned int control_word;
    int err;

  
	#ifdef _WIN32
		// Show original FP control word and do calculation.
		err = _controlfp_s(&control_word, 0, 0);
		if ( err ) {
			printf( "fpu error %d\n",err);
			
		}
	#else
		_FPU_GETCW(control_word);
	#endif

    printf( "FPU Control Word: 0x%.4x\n", control_word );
    printf( "%1.1f * %1.1f = %.15e\n", a, a, a * a );
    printf( "%1.1f * %1.1f = %.15e\n", f, f, f * f );
	
	return 0;
}

GMOD_MODULE_CLOSE( )
{
	return 0;
}