#include "gm_pvs.h"

GarrysMod::Lua::ILuaBase* g_Lua;
IVEngineServer *g_IVEngineServer = NULL;

int GetClusterCount( lua_State* state )
{
	LUA->PushNumber(g_IVEngineServer->GetClusterCount());
	return 1;
}

int GetClusterForOrigin( lua_State* state )
{
	LUA->CheckType(1,GarrysMod::Lua::Type::VECTOR);

	GarrysMod::Lua::UserData* obj = ( GarrysMod::Lua::UserData* )LUA->GetUserdata( 1 );

	LUA->PushNumber(g_IVEngineServer->GetClusterForOrigin(*reinterpret_cast<Vector*>( obj->data )));
	return 1;
}

int GetAllClusterBounds( lua_State* state )
{
	CUtlVector<bbox_t> clusterbounds;
	int clusterCount = g_IVEngineServer->GetClusterCount();
	clusterbounds.SetCount( clusterCount );
	g_IVEngineServer->GetAllClusterBounds(clusterbounds.Base(),clusterCount);

	LUA->CreateTable();
		LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB );
		for ( int i = 0; i < clusterCount; i++ )
		{
			LUA->PushNumber(i);
				LUA->CreateTable();
					LUA->GetField( -3, "Vector" );
						LUA->PushNumber(clusterbounds[i].mins.x);
							LUA->PushNumber(clusterbounds[i].mins.y);
								LUA->PushNumber(clusterbounds[i].mins.z);
					LUA->Call( 3, 1 );
				LUA->SetField( -2, "min" );
					LUA->GetField( -3, "Vector" );
						LUA->PushNumber(clusterbounds[i].maxs.x);
							LUA->PushNumber(clusterbounds[i].maxs.y);
								LUA->PushNumber(clusterbounds[i].maxs.z);
					LUA->Call( 3, 1 );
				LUA->SetField( -2, "max" );
		LUA->SetTable(-4);
		/*char buffer [33];
		itoa(i,buffer,10);
		LUA->SetField( -3, buffer );*/
		}
			
	LUA->Pop(1);
		//LUA->PushNumber(clusterCount);
		//LUA->SetField( -2, "2" );
	return 1;
}

int GetPVSForCluster( lua_State* state )
{
	LUA->CheckType(1,GarrysMod::Lua::Type::NUMBER);

	PVS* pvs = new PVS(state);
	pvs->pushObject();

	return 1;
}
//
// Called when you module is opened
//
GMOD_MODULE_OPEN()
{
	LuaOO::instance()->registerPollingFunction(state, "pvs::Poll");
	LuaOO::instance()->registerClasses(state);

	g_Lua = LUA;

	CreateInterfaceFn ENGINEFactory = Sys_GetFactory( ENGINE_LIB );
	if ( !ENGINEFactory )
		LUA->ThrowError( "gm_pvs: Error getting " ENGINE_LIB " factory.\n" );

	g_IVEngineServer = ( IVEngineServer* )ENGINEFactory( INTERFACEVERSION_VENGINESERVER, NULL );
	if ( !g_IVEngineServer )
		LUA->ThrowError( "gm_pvs: Error getting IVEngineServer interface.\n" );

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	int iGlobalTable = LUA->ReferenceCreate();

	int pvsTable = Lua::Table::Create();
		Lua::Table::Insert(pvsTable, "GetClusterCount", GetClusterCount);
		Lua::Table::Insert(pvsTable, "GetClusterForOrigin", GetClusterForOrigin);
		Lua::Table::Insert(pvsTable, "GetAllClusterBounds", GetAllClusterBounds);
		Lua::Table::Insert(pvsTable, "GetPVSForCluster", GetPVSForCluster);
	Lua::Table::Add(iGlobalTable, "pvs", pvsTable);

	LUA->ReferenceFree(iGlobalTable);

	return 0;
}

//
// Called when your module is closed
//
GMOD_MODULE_CLOSE()
{
	LuaOO::shutdown();
	return 0;
}
