#undef _UNICODE

#pragma comment(lib,"mathlib.lib")
#pragma comment(lib,"tier0.lib")
#pragma comment(lib,"tier1.lib")
#pragma comment(lib,"tier2.lib")
#pragma comment(lib,"vstdlib.lib")

#define GMMODULE
#include "GarrysMod\Lua\Interface.h"

#ifdef _WIN32

#define VPHYSICS_LIB "vphysics.dll"
#define DATACACHE_LIB "datacache.dll"
#define ENGINE_LIB "engine.dll"

#elif _LINUX

#define VPHYSICS_LIB "vphysics.so"
#define DATACACHE_LIB "datacache.so"
#define ENGINE_LIB "engine.so"

#endif

#define GAME_DLL
#include "cbase.h"
#include "vphysics_interface.h"
#include "datacache\imdlcache.h"
#include "physics_prop_ragdoll.h"
#include "vphysics\constraints.h"
#include "vcollide_parse.h"
#include "datacache\imdlcache.h"
#include "physics_shared.h"
#include "ragdoll_shared.h"

using namespace GarrysMod::Lua;

IMDLCache* mdlcache = NULL;

IPhysics* physics = NULL;
IPhysicsEnvironment* physenv = NULL;
IPhysicsCollision* physcollision = NULL;
IVEngineServer* engine = NULL;

ILuaBase* g_Lua = NULL;
CFunc funcVector;

#define Error(x) g_Lua->ThrowError(x);
#define LUA_FUNC(name) int name(lua_State* state)
#define REG_FUNC(name) LUA->PushCFunction(name); \
					   LUA->SetField(-2, #name)

bool IsRagdoll(CBaseEntity* ent)
{
	return dynamic_cast<CRagdollProp*>(ent) != NULL;
}

size_t GetData(datamap_t* map, const char* name)
{
	for (int i=0;i<map->dataNumFields;i++)
    {
        if(!map->dataDesc[i].fieldName || map->dataDesc[i].fieldType == FIELD_VOID)
		{
			continue;
		}
                 
        if (FStrEq(name, map->dataDesc[i].fieldName))
        {
			return map->dataDesc[i].fieldOffset[0];
        }
    }
    
	return NULL;
}

LUA_FUNC(RemoveConstraints);
LUA_FUNC(GetConstraintInfo);

GMOD_MODULE_OPEN()
{
	CreateInterfaceFn vphysics = Sys_GetFactory(VPHYSICS_LIB);

	if (!vphysics)
	{
		Error("gm_dolly: Couldn't create vphysics interface");
		return 0;
	}

	physics = (IPhysics*)vphysics(VPHYSICS_INTERFACE_VERSION, NULL);

	if (!physics)
	{
		Error("gm_dolly: Couldn't create IPhysics interface");
		return 0;
	}

	physenv = physics->GetActiveEnvironmentByIndex(0);
	
	physcollision = (IPhysicsCollision*)vphysics(VPHYSICS_COLLISION_INTERFACE_VERSION, NULL);

	if (!physcollision)
	{
		Error("gm_dolly: Couldn't create IPhysicsCollision interface");
		return 0;
	}

	CreateInterfaceFn datacache = Sys_GetFactory(DATACACHE_LIB);

	if (!datacache)
	{
		Error("gm_dolly: Couldn't create datacache interface");
		return 0;
	}

	mdlcache = (IMDLCache*)datacache(MDLCACHE_INTERFACE_VERSION, NULL);

	if (!mdlcache)
	{
		Error("gm_dolly: Couldn't create IMDLCache interface");
		return 0;
	}

	CreateInterfaceFn _engine = Sys_GetFactory(ENGINE_LIB);

	if (!_engine)
	{
		Error("gm_dolly: Couldn't create engine interface");
		return 0;
	}

	engine = (IVEngineServer*)_engine(INTERFACEVERSION_VENGINESERVER, NULL);

	if (!engine)
	{
		Error("Couldn't create IVEngineServer interface");
		return 0;
	}

	g_Lua = LUA;
	
	LUA->PushSpecial(SPECIAL_GLOB);
		REG_FUNC(RemoveConstraints);
		REG_FUNC(GetConstraintInfo);		
		LUA->GetField(-1, "Vector");
		funcVector = LUA->GetCFunction();		
	LUA->Pop(2);

	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}

CBaseEntity* GetEntity(CBaseHandle* handle)
{
	edict_t* edict = engine->PEntityOfEntIndex(handle->GetEntryIndex());
	if (!edict)
	{
		return NULL;
	}

	IServerUnknown* unknown = edict->GetUnknown();

	if (!unknown)
	{
		return NULL;
	}

	return unknown->GetBaseEntity();
}

LUA_FUNC(RemoveConstraints)
{
	LUA->CheckType(1, Type::ENTITY);
	
	CBaseHandle* handle = (CBaseHandle*)((UserData*)LUA->GetUserdata(1))->data;

	CBaseEntity* ent = GetEntity(handle);

	if (!ent || !IsRagdoll(ent))
	{
		Error("RemoveConstraints: Invalid entity");
		return 0;
	}

	datamap_t* datamap = ent->GetDataDescMap();
	
	size_t listCount_offset = GetData(datamap, "m_ragdoll.listCount");
	int listCount = *(int*)((size_t)ent+listCount_offset);
	
	for (int i=1;i<listCount;++i)
	{
		char str[31];
		sprintf(str, "m_ragdoll.list[%i].pConstraint", i);

		size_t pConstraint_offset = GetData(datamap, str);
		
		IPhysicsConstraint** ppConstraint = (IPhysicsConstraint**)((size_t)ent+pConstraint_offset);
		IPhysicsConstraint* pConstraint = *ppConstraint;

		if (pConstraint)
		{
			physenv->DestroyConstraint(pConstraint);
			*ppConstraint = NULL;
		}
	}
	
	size_t allowStretch_offset = GetData(datamap, "m_ragdoll.allowStretch");
	*(bool*)((size_t)ent+allowStretch_offset) = true;
	
	return 0;
}

void PushLuaVector(Vector vec)
{
	g_Lua->PushCFunction(funcVector);
	g_Lua->PushNumber(vec.x);
	g_Lua->PushNumber(vec.y);
	g_Lua->PushNumber(vec.z);
	g_Lua->Call(3, 1);
}

LUA_FUNC(GetConstraintInfo)
{
	LUA->CheckType(1, Type::STRING);

	const char* mdlName = LUA->GetString(1);

	MDLHandle_t mdlHandle = mdlcache->FindMDL(mdlName);

	vcollide_t* vcollide = mdlcache->GetVCollide(mdlHandle);
	
	IVPhysicsKeyParser* parse = physcollision->VPhysicsKeyParserCreate(vcollide->pKeyValues);

	LUA->CreateTable();
	
	int index = 1;

	while (!parse->Finished())
	{
		const char* blockName = parse->GetCurrentBlockName();

		if (FStrEq(blockName, "ragdollconstraint"))
		{
			LUA->PushNumber(index++);
			LUA->CreateTable();

			constraint_ragdollparams_t constraint;
			parse->ParseRagdollConstraint(&constraint, NULL);
			
			LUA->PushNumber(constraint.axes[0].minRotation);
			LUA->SetField(-2, "minRotationX");

			LUA->PushNumber(constraint.axes[1].minRotation);
			LUA->SetField(-2, "minRotationY");

			LUA->PushNumber(constraint.axes[2].minRotation);
			LUA->SetField(-2, "minRotationZ");

			LUA->PushNumber(constraint.axes[0].maxRotation);
			LUA->SetField(-2, "maxRotationX");

			LUA->PushNumber(constraint.axes[1].maxRotation);
			LUA->SetField(-2, "maxRotationY");

			LUA->PushNumber(constraint.axes[2].maxRotation);
			LUA->SetField(-2, "maxRotationZ");

			LUA->PushNumber(constraint.childIndex);
			LUA->SetField(-2, "childIndex");
						
			LUA->PushNumber(constraint.parentIndex);
			LUA->SetField(-2, "parentIndex");
						
			LUA->SetTable(-3);
		}		
		else
		{
			parse->SkipBlock();
		}
	}

	return 1;
}