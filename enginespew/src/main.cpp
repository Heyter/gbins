#ifdef _WIN32
	#include <Windows.h>
#else
	#include <pthread.h>
#endif

#include <tier0/dbg.h>
#include <Color.h>
#include <cstdio>
#include <GarrysMod/Lua/Interface.h>


SpewOutputFunc_t g_fnOldSpew = NULL;

// which thread
#ifdef _WIN32
	DWORD g_iMainThread;
#else
	pthread_t g_iMainThread;
#endif

lua_State* luas = NULL;
volatile bool inspew = false; // volatile not needed but itll remind me
SpewRetval_t LuaSpew( SpewType_t spewType, const char *pMsg )
{
	
	// test for wrong thread...
#ifdef _WIN32
	if ( GetCurrentThreadId() != g_iMainThread )
#else
	if ( pthread_self() != g_iMainThread )
#endif
		return g_fnOldSpew( spewType, pMsg );

	// no message...
	if ( !pMsg || inspew || !luas )
		return g_fnOldSpew( spewType, pMsg );

	// NOW WE ARE TALKING
	lua_State* state=luas;

	LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB );
		LUA->GetField( -1, "hook" );
			LUA->GetField( -1, "Run" );
				
				
				LUA->PushString( "EngineSpew" );
				LUA->PushNumber( (double) spewType  );
				LUA->PushString( pMsg );
				LUA->PushString( GetSpewOutputGroup()  );
				LUA->PushNumber( (double) GetSpewOutputLevel() );
				
				// screw tables. Lets just push this in like this and be done with it.
				const Color*  c = GetSpewOutputColor( );
				
					int Red, Green, Blue, Alpha = 0;
					c->GetColor(Red,Green,Blue,Alpha);

					LUA->PushNumber( (float) Red );
					LUA->PushNumber( (float) Green );
					LUA->PushNumber( (float) Blue );

				/*
					lua_run hook.Add("EngineSpew","a",function(t,msg,grp,lev,r,g,b) print(...) end)
					
				*/
				inspew = true;
				
				// hook.Run("EngineSpew",0,"","",0,0,0,0);
				if( LUA->PCall( 8, 1, 0 ) != 0 )
				{
					Warning( "[EngineSpew error] %s\n", LUA->GetString( ) );
					LUA->Pop( 3 );
					inspew = false;
					return g_fnOldSpew( spewType, pMsg );
				}
				
				if( LUA->IsType( -1, GarrysMod::Lua::Type::BOOL ) && LUA->GetBool( ) )
				{
					LUA->Pop( 3 );
					inspew = false;
					return SPEW_CONTINUE;
				}
				
			//LUA->Pop( 1 );
		//LUA->Pop( 1 );
	//LUA->Pop( 1 );
	LUA->Pop( 3 );
	
	inspew=false;
	
	return g_fnOldSpew( spewType, pMsg );
}


void keep_loaded() {
#ifdef _WIN32
	HMODULE ignore;
	BOOL wat = GetModuleHandleEx(
					GET_MODULE_HANDLE_EX_FLAG_PIN|GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, 
					(LPCSTR)&keep_loaded, 
					&ignore);
	if (!wat) {
		printf("ooo this is bad\n");
	}
#else
	// ????
#endif
}


void spew_hook() 
{
	static bool func_hooked = false;
	if (func_hooked) return;
	func_hooked = true;
	
	g_fnOldSpew = GetSpewOutputFunc();
	keep_loaded();
	SpewOutputFunc( LuaSpew );
}


GMOD_MODULE_OPEN( )
{
	luas = state;
	
	#ifdef _WIN32
		g_iMainThread = GetCurrentThreadId();
	#else
		g_iMainThread = pthread_self();
	#endif
	
	spew_hook();
	
	return 0;
}

GMOD_MODULE_CLOSE( )
{
	luas = NULL;
	return 0;
}



BOOLEAN WINAPI DllMain( IN HINSTANCE hDllHandle, 
         IN DWORD     nReason, 
         IN LPVOID    Reserved )
 {
  
  switch ( nReason )
   {
    case DLL_PROCESS_ATTACH:

      printf("DLL_PROCESS_ATTACH\n");

      break;

    case DLL_PROCESS_DETACH:
      printf("DLL_PROCESS_DETACH\n");

      break;
   }




  return TRUE;

}