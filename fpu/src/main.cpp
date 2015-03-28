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

static unsigned int
x87_get_control_word(void)
{
    

	static unsigned int mode;
    #ifdef _WIN32 
		__asm {
			STMXCSR mode
		} 
    
	#else
		__asm__ __volatile__("STMXCSR %0\n\t":"=m"(*&mode):);
    #endif 
	return mode;
}


GMOD_MODULE_OPEN( )
{

    double a = 0.1;
    float f = 0.1;
	double aa = 0.1111111111111111111111111;
    float ff = 0.111111111111111111111111;
	double aaa = 0.66666666666666666666666;
    float fff = 0.666666666666666666666666;
	
    unsigned int control_word;

	control_word = x87_get_control_word();

    printf( "FPU Control Word: %x\n", control_word );
    printf( "%1.1f * %1.1f = %.15e\n", a, a, a * a );
	
	printf( "conv %.15e (%.15e) -> %.15e\n", aa,ff,(float)aa );
	printf( "conv %.15e (%.15e) -> %.15e\n", aaa,fff,(float)aaa );
	
	return 0;
}

GMOD_MODULE_CLOSE( )
{
	return 0;
}