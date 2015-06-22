#include "gm_pvs.h"

PVS::PVS(lua_State* state)
	: LuaObjectBaseTemplate<PVS>(state)
{
	int pvsSize = ceil(g_IVEngineServer->GetClusterCount() / 8.0f); // bits to bytes
	pvs = new byte[pvsSize];
	int cluster = LUA->GetNumber(1);
	g_IVEngineServer->GetPVSForCluster(cluster, pvsSize, pvs);
}

PVS::~PVS(void)
{
	delete[] pvs;
}

BEGIN_BINDING(PVS)
	BIND_FUNCTION( "CheckCluster", PVS::CheckCluster )
	BIND_FUNCTION( "GetList", PVS::GetList )
	BIND_FUNCTION( "CheckOrigin", PVS::CheckOrigin )
	BIND_FUNCTION( "CheckBox", PVS::CheckBox )
END_BINDING();

int PVS::GetList()
{
	int pvsTable = Lua::Table::Create();
	int listCounter = 0;
	CUtlVector<bbox_t> clusterbounds;
	int clusterCount = g_IVEngineServer->GetClusterCount();
	clusterbounds.SetCount( clusterCount );
	g_IVEngineServer->GetAllClusterBounds(clusterbounds.Base(),clusterCount);

	for ( int i = 0; i < clusterCount; i++ )
	{
		Vector origin = ((clusterbounds[i].maxs - clusterbounds[i].mins)/2.0f) + clusterbounds[i].mins;

		if (g_IVEngineServer->CheckOriginInPVS(origin,pvs,clusterCount)) {
			Lua::Table::Insert(pvsTable,listCounter,i);
			listCounter++;
		}
	}

	return 1;
}

int PVS::CheckCluster()
{
	MLUA->CheckType(2,GarrysMod::Lua::Type::NUMBER);

	CUtlVector<bbox_t> clusterbounds;
	int clusterCount = g_IVEngineServer->GetClusterCount();
	clusterbounds.SetCount( clusterCount );
	g_IVEngineServer->GetAllClusterBounds(clusterbounds.Base(),clusterCount);

	int cluster = MLUA->GetNumber(2);
	if (cluster > g_IVEngineServer->GetClusterCount()) {
		MLUA->PushBool(false);
		return 1;
	}

	Vector origin = ((clusterbounds[cluster].maxs - clusterbounds[cluster].mins)/2.0f) + clusterbounds[cluster].mins;
	MLUA->PushBool(g_IVEngineServer->CheckOriginInPVS(origin,pvs,clusterCount));
	return 1;
}

int PVS::CheckOrigin()
{
	MLUA->CheckType(2,GarrysMod::Lua::Type::VECTOR);

	int clusterCount = g_IVEngineServer->GetClusterCount();
	GarrysMod::Lua::UserData* obj = ( GarrysMod::Lua::UserData* )MLUA->GetUserdata( 2 );

	MLUA->PushBool(g_IVEngineServer->CheckOriginInPVS(*reinterpret_cast<Vector*>( obj->data ),pvs,clusterCount));

	return 1;
}

int PVS::CheckBox()
{
	MLUA->CheckType(2,GarrysMod::Lua::Type::VECTOR);
	MLUA->CheckType(3,GarrysMod::Lua::Type::VECTOR);

	int clusterCount = g_IVEngineServer->GetClusterCount();

	GarrysMod::Lua::UserData* obj1 = ( GarrysMod::Lua::UserData* )MLUA->GetUserdata( 2 );
	GarrysMod::Lua::UserData* obj2 = ( GarrysMod::Lua::UserData* )MLUA->GetUserdata( 3 );

	MLUA->PushBool(g_IVEngineServer->CheckBoxInPVS(*reinterpret_cast<Vector*>( obj1->data ),*reinterpret_cast<Vector*>( obj2->data ),pvs,clusterCount));
	return 1;
}