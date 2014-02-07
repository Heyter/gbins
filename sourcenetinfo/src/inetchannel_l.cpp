#include <common/GMLuaModule.h>

#include <inetchannel.h>
#include <inetmessage.h>
#include "inetchannel_l.h"

extern ILuaInterface *gLua;

namespace MT_INetChannel
{
	LUA_FUNCTION(GetName)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->GetName());

		return 1;
	}

	LUA_FUNCTION(GetAddress)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->GetAddress());

		return 1;
	}

	LUA_FUNCTION(GetTime)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetTime());

		return 1;
	}

	LUA_FUNCTION(GetTimeConnected)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetTimeConnected());

		return 1;
	}

	LUA_FUNCTION(GetBufferSize)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetBufferSize());

		return 1;
	}

	LUA_FUNCTION(GetDataRate)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetDataRate());

		return 1;
	}

	LUA_FUNCTION(IsLoopback)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->IsLoopback());

		return 1;
	}

	LUA_FUNCTION(IsTimingOut)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->IsTimingOut());

		return 1;
	}

	LUA_FUNCTION(IsPlayback)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->IsPlayback());

		return 1;
	}

	LUA_FUNCTION(GetLatency)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetLatency(gLua->GetInteger(2)));

		return 1;
	}

	LUA_FUNCTION(GetAvgLatency)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetAvgLatency(gLua->GetInteger(2)));

		return 1;
	}

	LUA_FUNCTION(GetAvgLoss)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetAvgLoss(gLua->GetInteger(2)));

		return 1;
	}

	LUA_FUNCTION(GetAvgChoke)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetAvgChoke(gLua->GetInteger(2)));

		return 1;
	}

	LUA_FUNCTION(GetAvgData)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetAvgData(gLua->GetInteger(2)));

		return 1;
	}

	LUA_FUNCTION(GetAvgPackets)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetAvgPackets(gLua->GetInteger(2)));

		return 1;
	}

	LUA_FUNCTION(GetTotalData)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetTotalData(gLua->GetInteger(2)));

		return 1;
	}

	LUA_FUNCTION(GetSequenceNr)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetSequenceNr(gLua->GetInteger(2)));

		return 1;
	}

	LUA_FUNCTION(IsValidPacket)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);
		gLua->CheckType(3, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->IsValidPacket(gLua->GetInteger(2), gLua->GetInteger(3)));

		return 1;
	}

	LUA_FUNCTION(GetPacketTime)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);
		gLua->CheckType(3, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetPacketTime(gLua->GetInteger(2), gLua->GetInteger(3)));

		return 1;
	}

	LUA_FUNCTION(GetPacketBytes)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);
		gLua->CheckType(3, Type::NUMBER);
		gLua->CheckType(4, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetPacketBytes(gLua->GetInteger(2), gLua->GetInteger(3), gLua->GetInteger(4)));

		return 1;
	}

	LUA_FUNCTION(GetStreamProgress)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		int received = 0, total = 0;
		
		if (channel->GetStreamProgress(gLua->GetInteger(2), &received, &total))
		{
			ILuaObject *returnT  = gLua->GetNewTable();
				returnT->SetMember("received", (float)received);
				returnT->SetMember("total", (float)total);
				
				gLua->Push(returnT);
			returnT->UnReference();
		}
		else
		{
			gLua->PushNil();
		}

		return 1;
	}

	LUA_FUNCTION(GetTimeSinceLastReceived)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetTimeSinceLastReceived());

		return 1;
	}

	LUA_FUNCTION(GetCommandInterpolationAmount)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);
		gLua->CheckType(3, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetCommandInterpolationAmount(gLua->GetInteger(2), gLua->GetInteger(3)));

		return 1;
	}

	LUA_FUNCTION(GetPacketResponseLatency)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);
		gLua->CheckType(3, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		int pnLatencyMsecs = 0, pnChoke = 0;

		channel->GetPacketResponseLatency(gLua->GetInteger(2), gLua->GetInteger(3), &pnLatencyMsecs, &pnChoke);

		ILuaObject *returnT = gLua->GetNewTable();
			returnT->SetMember("latency", (float)pnLatencyMsecs);
			returnT->SetMember("choke", (float)pnChoke);

			gLua->Push(returnT);
		returnT->UnReference();

		return 1;
	}

	LUA_FUNCTION(GetRemoteFramerate)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		float pflFrameTime = 0.0f, pflFrameTimeStdDeviation = 0.0f;

		channel->GetRemoteFramerate(&pflFrameTime, &pflFrameTimeStdDeviation);

		ILuaObject *returnT = gLua->GetNewTable();
			returnT->SetMember("frametime", pflFrameTime);
			returnT->SetMember("frametimestddeviation", pflFrameTimeStdDeviation);

			gLua->Push(returnT);
		returnT->UnReference();

		return 1;
	}

	LUA_FUNCTION(GetTimeoutSeconds)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetTimeoutSeconds());

		return 1;
	}

	LUA_FUNCTION(SetDataRate)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->SetDataRate(gLua->GetDouble(2));

		return 0;
	}

	LUA_FUNCTION(StartStreaming)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->StartStreaming(gLua->GetInteger(2)));

		return 1;
	}

	LUA_FUNCTION(ResetStreaming)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->ResetStreaming();

		return 0;
	}

	LUA_FUNCTION(SetTimeout)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->SetTimeout(gLua->GetDouble(2));

		return 0;
	}

	LUA_FUNCTION(SetChallengeNr)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->SetChallengeNr(gLua->GetInteger(2));

		return 0;
	}

	LUA_FUNCTION(Reset)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->Reset();

		return 0;
	}

	LUA_FUNCTION(Clear)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->Clear();

		return 0;
	}

	LUA_FUNCTION(Shutdown)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::STRING);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->Shutdown(gLua->GetString(2));

		return 0;
	}

	LUA_FUNCTION(ProcessPlayback)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->ProcessPlayback();

		return 0;
	}

	LUA_FUNCTION(ProcessStream)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->ProcessStream());

		return 1;
	}

	LUA_FUNCTION(SendFile)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::STRING);
		gLua->CheckType(3, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->SendFile(gLua->GetString(2), gLua->GetInteger(3)));

		return 1;
	}

	LUA_FUNCTION(DenyFile)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::STRING);
		gLua->CheckType(3, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->DenyFile(gLua->GetString(2), gLua->GetInteger(3));

		return 0;
	}

	LUA_FUNCTION(SetChoked)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->SetChoked();

		return 0;
	}

	LUA_FUNCTION(Transmit)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::BOOL);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->Transmit(gLua->GetBool(2)));

		return 1;
	}

	LUA_FUNCTION(GetDropNumber)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetDropNumber());

		return 1;
	}

	LUA_FUNCTION(GetSocket)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetSocket());

		return 1;
	}

	LUA_FUNCTION(GetChallengeNr)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetChallengeNr());

		return 1;
	}

	LUA_FUNCTION(GetSequenceData)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		int nOutSequenceNr = 0, nInSequenceNr = 0, nOutSequenceNrAck = 0;

		channel->GetSequenceData(nOutSequenceNr, nInSequenceNr, nOutSequenceNrAck);

		ILuaObject *returnT = gLua->GetNewTable();
			returnT->SetMember("outsequencenr", (float)nOutSequenceNr);
			returnT->SetMember("insequencenr", (float)nInSequenceNr);
			returnT->SetMember("outsequencenrack", (float)nOutSequenceNrAck);
			
			gLua->Push(returnT);
		returnT->UnReference();

		return 1;
	}

	LUA_FUNCTION(SetSequenceData)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);
		gLua->CheckType(3, Type::NUMBER);
		gLua->CheckType(4, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->SetSequenceData(gLua->GetInteger(2), gLua->GetInteger(3), gLua->GetInteger(4));

		return 0;
	}

	LUA_FUNCTION(UpdateMessageStats)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);
		gLua->CheckType(3, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->UpdateMessageStats(gLua->GetInteger(2), gLua->GetInteger(3));

		return 0;
	}

	LUA_FUNCTION(CanPacket)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->CanPacket());

		return 1;
	}

	LUA_FUNCTION(IsOverflowed)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->IsOverflowed());

		return 1;
	}

	LUA_FUNCTION(IsTimedOut)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->IsTimedOut());

		return 1;
	}

	LUA_FUNCTION(HasPendingReliableData)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->HasPendingReliableData());

		return 1;
	}

	LUA_FUNCTION(SetFileTransmissionMode)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::BOOL);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->SetFileTransmissionMode(gLua->GetBool(2));

		return 0;
	}

	LUA_FUNCTION(SetCompressionMode)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::BOOL);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->SetCompressionMode(gLua->GetBool(2));

		return 0;
	}

	LUA_FUNCTION(RequestFile)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::STRING);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->RequestFile(gLua->GetString(2)));

		return 1;
	}
	
	LUA_FUNCTION(ReplicateData)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::STRING);
		gLua->CheckType(3, Type::STRING);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);


		char pckBuf[256];
		bf_write pck( pckBuf, sizeof( pckBuf ) );
		
		pck.WriteUBitLong( 5, 6 );
		pck.WriteByte( 0x01 );
		pck.WriteString( gLua->GetString(2) );
		pck.WriteString( gLua->GetString(3) );

		channel->SendData ( pck );
	

		return 0;
	}

	LUA_FUNCTION(SetMaxBufferSize)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::BOOL);
		gLua->CheckType(3, Type::NUMBER);
		gLua->CheckType(4, Type::BOOL);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->SetMaxBufferSize(gLua->GetBool(2), gLua->GetInteger(3), gLua->GetBool(4));

		return 0;
	}

	LUA_FUNCTION(IsNull)
	{
		gLua->CheckType(1, INETCHANNEL_ID);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push(channel->IsNull());

		return 1;
	}

	LUA_FUNCTION(GetNumBitsWritten)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::BOOL);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetNumBitsWritten(gLua->GetBool(2)));

		return 1;
	}

	LUA_FUNCTION(SetInterpolationAmount)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->SetInterpolationAmount(gLua->GetDouble(2));

		return 0;
	}

	LUA_FUNCTION(SetRemoteFramerate)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);
		gLua->CheckType(3, Type::NUMBER);

		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->SetRemoteFramerate(gLua->GetDouble(2), gLua->GetDouble(3));

		return 0;
	}

	LUA_FUNCTION(SetMaxRoutablePayloadSize)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		gLua->CheckType(2, Type::NUMBER);
		
		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		channel->SetMaxRoutablePayloadSize(gLua->GetInteger(2));

		return 0;
	}

	LUA_FUNCTION(GetMaxRoutablePayloadSize)
	{
		gLua->CheckType(1, INETCHANNEL_ID);
		
		INetChannel *channel = (INetChannel *)gLua->GetUserData(1);

		gLua->Push((double)channel->GetMaxRoutablePayloadSize());

		return 1;
	}
}