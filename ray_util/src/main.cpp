#include "interface.h"
#include "ivrenderview.h"
#include "eifacev21.h"
#include "game/server/iplayerinfo.h"
#include "vphysics_interface.h"

#include "cbaseserver.h"

//#define GMMODULE
#include "GarrysMod/Lua/Interface.h"

#ifdef _WIN32
	const char enginepath[] = "engine.dll";
	const char serverpath[] = "server.dll";
	const char vphysicspath[] = "vphysics.dll";
#else
	const char enginepath[] = "engine_srv.so";
	const char serverpath[] = "garrysmod/bin/server_srv.so";
	const char vphysicspath[] = "vphysics_srv.so";

	#include <dlfcn.h>
	#include "memutils.h"
#endif

VEngineServerV21::IVEngineServer *engineserver;
float *absoluteframetime;
IPhysics *physics;
CBaseServer *baseserver;

int ServerCommand(lua_State *state)
{
	engineserver->ServerCommand(LUA->CheckString(1));
	return 0;
}

int FrameTime(lua_State *state)
{
	LUA->PushNumber(*absoluteframetime);
	return 1;
}

int PhysEnvSimulate(lua_State *state)
{
	IPhysicsEnvironment *physenv = physics->GetActiveEnvironmentByIndex(0);
	if (physenv)
		physenv->Simulate(DEFAULT_TICK_INTERVAL);
	else
		LUA->ThrowError("PhysEnv doesn't exist.");
	return 0;
}

#ifndef _WIN32
	int GetNumClients(lua_State *state)
	{
		LUA->PushNumber(baseserver->GetNumClients());
		return 1;
	}

	int GetNumFakeClients(lua_State *state)
	{
		LUA->PushNumber(baseserver->GetNumFakeClients());
		return 1;
	}
#endif

GMOD_MODULE_OPEN()
{
	CreateInterfaceFn enginedll = Sys_GetFactory(enginepath);
	engineserver = (VEngineServerV21::IVEngineServer*)enginedll(VENGINESERVER_INTERFACEVERSION_21, 0);

	CreateInterfaceFn serverdll = Sys_GetFactory(serverpath);
	IPlayerInfoManager *playerinfomanager = (IPlayerInfoManager*)serverdll(INTERFACEVERSION_PLAYERINFOMANAGER, 0);
	CGlobalVars *globalvars = playerinfomanager->GetGlobalVars();
	absoluteframetime = &globalvars->absoluteframetime;

	CreateInterfaceFn vphysicsdll = Sys_GetFactory(vphysicspath);
	physics = (IPhysics*)vphysicsdll(VPHYSICS_INTERFACE_VERSION, 0);

	#ifndef _WIN32
		void *dlengine = dlopen(enginepath, RTLD_LAZY);
		baseserver = (CBaseServer*)ResolveSymbol(dlengine, "sv");
		dlclose(dlengine);
	#endif

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "ray");
			LUA->PushCFunction(ServerCommand);
			LUA->SetField(-2, "ServerCommand");
			LUA->PushCFunction(FrameTime);
			LUA->SetField(-2, "FrameTime");
			LUA->PushCFunction(PhysEnvSimulate);
			LUA->SetField(-2, "PhysEnvSimulate");
			#ifndef _WIN32
				LUA->PushCFunction(GetNumClients);
				LUA->SetField(-2, "GetNumClients");
				LUA->PushCFunction(GetNumFakeClients);
				LUA->SetField(-2, "GetNumFakeClients");
			#endif
	LUA->Pop(2);

	return 0;
}

GMOD_MODULE_CLOSE()
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "ray");
			LUA->PushNil();
			LUA->SetField(-2, "ServerCommand");
			LUA->PushNil();
			LUA->SetField(-2, "FrameTime");
			LUA->PushNil();
			LUA->SetField(-2, "PhysEnvSimulate");
			#ifndef _WIN32
				LUA->PushNil();
				LUA->SetField(-2, "GetNumClients");
				LUA->PushNil();
				LUA->SetField(-2, "GetNumFakeClients");
			#endif
	LUA->Pop(2);

	engineserver = 0;
	absoluteframetime = 0;
	physics = 0;

	return 0;
}