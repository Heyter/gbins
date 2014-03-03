#undef _UNICODE

#include "GarrysMod\Lua\Interface.h"

#include "cbase.h"
#include "vphysics_interface.h"
#include "datacache\imdlcache.h"
#include "physics_prop_ragdoll.h"
#include "vphysics\constraints.h"
#include "physics_saverestore.h"
#include "vcollide_parse.h"
#include "vphysics\collision_set.h"
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

bool IsRagdoll(CBaseEntity* pEnt)
{
	return dynamic_cast<CRagdollProp*>(pEnt) != NULL;
}

size_t GetData(datamap_t* pMap, const char* szName)
{
	for (int i=0;i<pMap->dataNumFields;i++)
    {
        if(!pMap->dataDesc[i].fieldName || pMap->dataDesc[i].fieldType == FIELD_VOID)
		{
			continue;
		}
                 
        if (FStrEq(szName, pMap->dataDesc[i].fieldName))
        {
			return pMap->dataDesc[i].fieldOffset[0];
        }
    }
    
	return NULL;
}

LUA_FUNC(RemoveInternalConstraint);
LUA_FUNC(AddInternalConstraint);
LUA_FUNC(GetModelInfo);
LUA_FUNC(RemovePhysicsObject);

GMOD_MODULE_OPEN()
{
	CreateInterfaceFn i_vphysics = Sys_GetFactory("vphysics.dll");

	if (!i_vphysics)
	{
		Error("FUCK");
		return 0;
	}

	physics = (IPhysics*)i_vphysics(VPHYSICS_INTERFACE_VERSION, NULL);

	if (!physics)
	{
		Error("DAMMIT");
		return 0;
	}

	physenv = physics->GetActiveEnvironmentByIndex(0);
	
	physcollision = (IPhysicsCollision*)i_vphysics(VPHYSICS_COLLISION_INTERFACE_VERSION, NULL);

	if (!physcollision)
	{
		Error("SHIT");
		return 0;
	}

	CreateInterfaceFn i_datacache = Sys_GetFactory("datacache.dll");

	if (!i_datacache)
	{
		Error("DANG");
		return 0;
	}

	mdlcache = (IMDLCache*)i_datacache(MDLCACHE_INTERFACE_VERSION, NULL);

	if (!mdlcache)
	{
		Error("NO MDL FOR U");
		return 0;
	}

	CreateInterfaceFn i_engine = Sys_GetFactory("engine.dll");

	if (!i_engine)
	{
		Error("SHIT ENGINE");
		return 0;
	}

	engine = (IVEngineServer*)i_engine(INTERFACEVERSION_VENGINESERVER, NULL);

	if (!engine)
	{
		Error("Fuck server");
		return 0;
	}

	g_Lua = LUA;
	
	LUA->PushSpecial(SPECIAL_REG);
		LUA->GetField(-1, "Entity");
		REG_FUNC(RemoveInternalConstraint);
	LUA->Pop(2);

	LUA->PushSpecial(SPECIAL_GLOB);			
		REG_FUNC(GetModelInfo);
				
		LUA->GetField(-1, "Vector");
		funcVector = LUA->GetCFunction();		
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}

CBaseEntity* GetEntity(CBaseHandle* pHandle)
{
	edict_t* pEdict = engine->PEntityOfEntIndex(pHandle->GetEntryIndex());
	if (!pEdict)
	{
		return NULL;
	}

	IServerUnknown* pUnknown = pEdict->GetUnknown();

	if (!pUnknown)
	{
		return NULL;
	}

	return pUnknown->GetBaseEntity();
}

LUA_FUNC(RemoveInternalConstraint)
{
	LUA->CheckType(1, Type::ENTITY);
	LUA->CheckType(2, Type::NUMBER);

	CBaseHandle* pHandle = (CBaseHandle*)((UserData*)LUA->GetUserdata(1))->data;

	CBaseEntity* pEnt = GetEntity(pHandle);

	if (!pEnt || !IsRagdoll(pEnt))
	{
		Error("Not a ragdoll");
		return 0;
	}

	int iIndex = ((int)LUA->GetNumber(2));

	if (iIndex < 1 || iIndex > 23)
	{
		Error("Index out of bounds");
		return 0;
	}

	char szStr[31];

	sprintf(szStr, "m_ragdoll.list[%i].pConstraint", iIndex);

	datamap_t* pDataMap = pEnt->GetDataDescMap();

	size_t offset = GetData(pDataMap, "m_ragdoll.allowStretch");

	*(bool*)((size_t)pEnt+offset) = true; //Elastagirl
	
	offset = GetData(pDataMap, szStr);

	if (!offset)
	{
		Error("Invalid offset");
		return 0;
	}

	IPhysicsConstraint** ppConstraint = (IPhysicsConstraint**)((size_t)pEnt+offset);
	IPhysicsConstraint* pConstraint = *ppConstraint;
	
	if (!pConstraint)
	{
		Error("Invalid constraint");
		return 0;
	}

	physenv->DestroyConstraint(pConstraint);	
	*ppConstraint = NULL;
	
	return 0;
}

void PushLuaVector(Vector vVec)
{
	g_Lua->PushCFunction(funcVector);
	g_Lua->PushNumber(vVec.x);
	g_Lua->PushNumber(vVec.y);
	g_Lua->PushNumber(vVec.z);
	g_Lua->Call(3, 1);
}

LUA_FUNC(GetModelInfo)
{
	LUA->CheckType(1, Type::STRING);

	const char* szMDLName = LUA->GetString(1);

	MDLHandle_t MDLHandle = mdlcache->FindMDL(szMDLName);

	vcollide_t* pVCollide = mdlcache->GetVCollide(MDLHandle);
	
	IVPhysicsKeyParser *pParse = physcollision->VPhysicsKeyParserCreate(pVCollide->pKeyValues);

	LUA->CreateTable();
	
	int iIndex = 1;

#define INIT_TABLE() LUA->PushNumber(iIndex++); \
			LUA->CreateTable(); \
			LUA->PushString(szBlockName); \
			LUA->SetField(-2, "type")

	while (!pParse->Finished())
	{
		const char* szBlockName = pParse->GetCurrentBlockName();

		if (FStrEq(szBlockName, "solid"))
		{
			INIT_TABLE();

			solid_t solid;
			pParse->ParseSolid(&solid, NULL);
			
			LUA->PushNumber(solid.index);
			LUA->SetField(-2, "index");

			PushLuaVector(solid.massCenterOverride);
			LUA->SetField(-2, "massCenterOverride");

			LUA->PushString(solid.name);
			LUA->SetField(-2, "name");

			objectparams_t params = solid.params;
			
			LUA->PushNumber(params.damping);
			LUA->SetField(-2, "damping");

			LUA->PushNumber(params.dragCoefficient);
			LUA->SetField(-2, "dragCoefficient");

			LUA->PushBool(params.enableCollisions);
			LUA->SetField(-2, "enableCollisions");

			LUA->PushNumber(params.inertia);
			LUA->SetField(-2, "inertia");

			LUA->PushNumber(params.mass);
			LUA->SetField(-2, "mass");

			LUA->PushNumber(params.rotdamping);
			LUA->SetField(-2, "rotdamping");

			LUA->PushNumber(params.rotInertiaLimit);
			LUA->SetField(-2, "rotInertiaLimit");

			LUA->PushNumber(params.volume);
			LUA->SetField(-2, "volume");

			LUA->SetTable(-3);
		}
		else if (FStrEq(szBlockName, "ragdollconstraint"))
		{
			INIT_TABLE();

			constraint_ragdollparams_t constraint;
			pParse->ParseRagdollConstraint(&constraint, NULL);
			
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

			LUA->PushBool(constraint.onlyAngularLimits);
			LUA->SetField(-2, "onlyAngularLimits");

			LUA->PushNumber(constraint.parentIndex);
			LUA->SetField(-2, "parentIndex");

			LUA->PushBool(constraint.useClockwiseRotations);
			LUA->SetField(-2, "useClockwiseRotations");

			LUA->SetTable(-3);
		}		
		else
		{
			pParse->SkipBlock();
		}
	}

	return 1;
}