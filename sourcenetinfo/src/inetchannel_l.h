#ifndef INETCHANNEL_L_H
#define INETCHANNEL_L_H

#define INETCHANNEL_NAME "INetChannel"
#define INETCHANNEL_ID 222

namespace MT_INetChannel
{
	LUA_FUNCTION(GetName);
	LUA_FUNCTION(GetAddress);
	LUA_FUNCTION(GetTime);
	LUA_FUNCTION(GetTimeConnected);
	LUA_FUNCTION(GetBufferSize);
	LUA_FUNCTION(GetDataRate);
	LUA_FUNCTION(IsLoopback);
	LUA_FUNCTION(IsTimingOut);
	LUA_FUNCTION(IsPlayback);
	LUA_FUNCTION(GetLatency);
	LUA_FUNCTION(GetAvgLatency);
	LUA_FUNCTION(GetAvgLoss);
	LUA_FUNCTION(GetAvgChoke);
	LUA_FUNCTION(GetAvgData);
	LUA_FUNCTION(GetAvgPackets);
	LUA_FUNCTION(GetTotalData);
	LUA_FUNCTION(GetSequenceNr);
	LUA_FUNCTION(IsValidPacket);
	LUA_FUNCTION(GetPacketTime);
	LUA_FUNCTION(GetPacketBytes);
	LUA_FUNCTION(GetStreamProgress);
	LUA_FUNCTION(GetTimeSinceLastReceived);
	LUA_FUNCTION(GetCommandInterpolationAmount);
	LUA_FUNCTION(GetPacketResponseLatency);
	LUA_FUNCTION(GetRemoteFramerate);
	LUA_FUNCTION(GetTimeoutSeconds);
	LUA_FUNCTION(SetDataRate);
	LUA_FUNCTION(StartStreaming);
	LUA_FUNCTION(ResetStreaming);
	LUA_FUNCTION(SetTimeout);
	LUA_FUNCTION(SetChallengeNr);
	LUA_FUNCTION(Reset);
	LUA_FUNCTION(Clear);
	LUA_FUNCTION(Shutdown);
	LUA_FUNCTION(ProcessPlayback);
	LUA_FUNCTION(ProcessStream);
	LUA_FUNCTION(SendFile);
	LUA_FUNCTION(DenyFile);
	LUA_FUNCTION(SetChoked);
	LUA_FUNCTION(Transmit);
	LUA_FUNCTION(GetDropNumber);
	LUA_FUNCTION(GetSocket);
	LUA_FUNCTION(GetChallengeNr);
	LUA_FUNCTION(GetSequenceData);
	LUA_FUNCTION(SetSequenceData);
	LUA_FUNCTION(UpdateMessageStats);
	LUA_FUNCTION(CanPacket);
	LUA_FUNCTION(IsOverflowed);
	LUA_FUNCTION(IsTimedOut);
	LUA_FUNCTION(HasPendingReliableData);
	LUA_FUNCTION(SetFileTransmissionMode);
	LUA_FUNCTION(SetCompressionMode);
	LUA_FUNCTION(RequestFile);
	LUA_FUNCTION(SetMaxBufferSize);
	LUA_FUNCTION(IsNull);
	LUA_FUNCTION(GetNumBitsWritten);
	LUA_FUNCTION(SetInterpolationAmount);
	LUA_FUNCTION(SetRemoteFramerate);
	LUA_FUNCTION(SetMaxRoutablePayloadSize);
	LUA_FUNCTION(GetMaxRoutablePayloadSize);
	LUA_FUNCTION(ReplicateData);
}

#endif //INETCHANNEL_L_H