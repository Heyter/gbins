

#ifdef _LINUX
#define ENGINE_LIB "engine_srv.so"
#else
#define ENGINE_LIB "engine.dll"
#endif


#include <eiface.h>
#include <cdll_int.h>
#include <inetchannelinfo.h>
#include <inetchannel.h>

#include "ILuaModuleManager.h"
#include "inetchannel_l.h"

GMOD_MODULE(Open, Close)

ILuaInterface *gLua = NULL;

IVEngineServer *sv_engine = NULL;

int GetPlayerEIndex(ILuaObject *playerEntity)
{
	ILuaObject *entityMT = gLua->GetMetaTable("Entity", Type::ENTITY);
		ILuaObject *entityM = entityMT->GetMember("EntIndex");
			entityM->Push();
					
			playerEntity->Push();

			gLua->Call(1, 1);
		entityM->UnReference();
	entityMT->UnReference();

	ILuaObject *returnL = gLua->GetReturn(0);

	int index = returnL->GetInt();

	returnL->UnReference();

	return index;
}

LUA_FUNCTION(SV_GetNetChannel)
{
	gLua->CheckType(1, Type::ENTITY);

	INetChannel *playerNC = (INetChannel *)sv_engine->GetPlayerNetInfo(GetPlayerEIndex(gLua->GetObject(1)));
	
	if (playerNC)
	{
		ILuaObject *metaT = gLua->GetMetaTable(INETCHANNEL_NAME, INETCHANNEL_ID);
			gLua->PushUserData(metaT, playerNC, INETCHANNEL_ID);
		metaT->UnReference();
	}
	else
	{
		gLua->PushNil();
	}

	return 1;
}

LUA_FUNCTION(SV_NetChannel)
{
	gLua->CheckType(1, Type::NUMBER);
	
	INetChannel *playerNC = (INetChannel *)sv_engine->GetPlayerNetInfo(gLua->GetInteger(1));
	
	if (playerNC)
	{
		ILuaObject *metaT = gLua->GetMetaTable(INETCHANNEL_NAME, INETCHANNEL_ID);
			gLua->PushUserData(metaT, playerNC, INETCHANNEL_ID);
		metaT->UnReference();
	}
	else
	{
		gLua->PushNil();
	}

	return 1;
	
}

int Open(lua_State *L)
{
	gLua = Lua();
	
	CreateInterfaceFn engineFactory = Sys_GetFactory(ENGINE_LIB);

	sv_engine = (IVEngineServer *)engineFactory(INTERFACEVERSION_VENGINESERVER, NULL);

	if (!sv_engine) {
		printf("THIS IS BAD: %s\n",INTERFACEVERSION_VENGINESERVER);
		return 0;
	}
	ILuaObject *metaT, *__index;

	metaT = gLua->GetMetaTable(INETCHANNEL_NAME, INETCHANNEL_ID);
		__index = gLua->GetNewTable();
			//INetChannelInfo

			__index->SetMember("GetName", MT_INetChannel::GetName);
			__index->SetMember("GetAddress", MT_INetChannel::GetAddress);
			__index->SetMember("GetTime", MT_INetChannel::GetTime);
			__index->SetMember("GetTimeConnected", MT_INetChannel::GetTimeConnected);
			__index->SetMember("GetBufferSize", MT_INetChannel::GetBufferSize);
			__index->SetMember("GetDataRate", MT_INetChannel::GetDataRate);
			
			__index->SetMember("IsLoopback", MT_INetChannel::IsLoopback);
			__index->SetMember("IsTimingOut", MT_INetChannel::IsTimingOut);
			__index->SetMember("IsPlayback", MT_INetChannel::IsPlayback);

			__index->SetMember("GetLatency", MT_INetChannel::GetLatency);
			__index->SetMember("GetAvgLatency", MT_INetChannel::GetAvgLatency);
			__index->SetMember("GetAvgLoss", MT_INetChannel::GetAvgLoss);
			__index->SetMember("GetAvgChoke", MT_INetChannel::GetAvgChoke);
			__index->SetMember("GetAvgData", MT_INetChannel::GetAvgData);
			__index->SetMember("GetAvgPackets", MT_INetChannel::GetAvgPackets);
			__index->SetMember("GetTotalData", MT_INetChannel::GetTotalData);
			__index->SetMember("GetSequenceNr", MT_INetChannel::GetSequenceNr);
			__index->SetMember("IsValidPacket", MT_INetChannel::IsValidPacket);
			__index->SetMember("GetPacketTime", MT_INetChannel::GetPacketTime);
			__index->SetMember("GetPacketBytes", MT_INetChannel::GetPacketBytes);
			__index->SetMember("GetStreamProgress", MT_INetChannel::GetStreamProgress);
			__index->SetMember("GetTimeSinceLastReceived", MT_INetChannel::GetTimeSinceLastReceived);
			__index->SetMember("GetCommandInterpolationAmount", MT_INetChannel::GetCommandInterpolationAmount);
			__index->SetMember("GetPacketResponseLatency", MT_INetChannel::GetPacketResponseLatency);
			__index->SetMember("GetRemoteFramerate", MT_INetChannel::GetRemoteFramerate);

			__index->SetMember("GetTimeoutSeconds", MT_INetChannel::GetTimeoutSeconds);

			//INetChannel

			__index->SetMember("SetDataRate", MT_INetChannel::SetDataRate);
			__index->SetMember("StartStreaming", MT_INetChannel::StartStreaming);
			__index->SetMember("ResetStreaming", MT_INetChannel::ResetStreaming);
			__index->SetMember("SetTimeout", MT_INetChannel::SetTimeout);
			__index->SetMember("SetChallengeNr", MT_INetChannel::SetChallengeNr);
			
			__index->SetMember("Reset", MT_INetChannel::Reset);
			__index->SetMember("Clear", MT_INetChannel::Clear);
			__index->SetMember("Shutdown", MT_INetChannel::Shutdown);
			
			__index->SetMember("ProcessPlayback", MT_INetChannel::ProcessPlayback);
			__index->SetMember("ProcessStream", MT_INetChannel::ProcessStream);

			__index->SetMember("SendFile", MT_INetChannel::SendFile);
			__index->SetMember("DenyFile", MT_INetChannel::DenyFile);
			__index->SetMember("SetChoked", MT_INetChannel::SetChoked);
			__index->SetMember("Transmit", MT_INetChannel::Transmit);

			__index->SetMember("GetDropNumber", MT_INetChannel::GetDropNumber);
			__index->SetMember("GetSocket", MT_INetChannel::GetSocket);
			__index->SetMember("GetChallengeNr", MT_INetChannel::GetChallengeNr);
			__index->SetMember("GetSequenceData", MT_INetChannel::GetSequenceData);
			__index->SetMember("SetSequenceData", MT_INetChannel::SetSequenceData);
				
			__index->SetMember("UpdateMessageStats", MT_INetChannel::UpdateMessageStats);
			__index->SetMember("CanPacket", MT_INetChannel::CanPacket);
			__index->SetMember("IsOverflowed", MT_INetChannel::IsOverflowed);
			__index->SetMember("IsTimedOut", MT_INetChannel::IsTimedOut);
			__index->SetMember("HasPendingReliableData", MT_INetChannel::HasPendingReliableData);

			__index->SetMember("SetFileTransmissionMode", MT_INetChannel::SetFileTransmissionMode);
			__index->SetMember("SetCompressionMode", MT_INetChannel::SetCompressionMode);
			__index->SetMember("RequestFile", MT_INetChannel::RequestFile);

			__index->SetMember("SetMaxBufferSize", MT_INetChannel::SetMaxBufferSize);

			__index->SetMember("IsNull", MT_INetChannel::IsNull);
			__index->SetMember("GetNumBitsWritten", MT_INetChannel::GetNumBitsWritten);
			__index->SetMember("SetInterpolationAmount", MT_INetChannel::SetInterpolationAmount);
			__index->SetMember("SetRemoteFramerate", MT_INetChannel::SetRemoteFramerate);

			__index->SetMember("SetMaxRoutablePayloadSize", MT_INetChannel::SetMaxRoutablePayloadSize);
			__index->SetMember("GetMaxRoutablePayloadSize", MT_INetChannel::GetMaxRoutablePayloadSize);
			__index->SetMember("ReplicateData", MT_INetChannel::ReplicateData);

			metaT->SetMember("__index", __index);
		__index->UnReference();
	metaT->UnReference();



	gLua->SetGlobal("NetChannel", SV_NetChannel);
	gLua->SetGlobal("GetNetChannel", SV_GetNetChannel);


	return 0;
}

int Close(lua_State *L)
{
	return 0;
}
