#include "netadr.h"
#include "iserver.h"

typedef void T;

typedef void CBaseClient;
typedef void CClientFrame;
typedef void CFrameSnapshot;

class CBaseServer : public IServer
{
public:
	virtual T GetCPUUsage() = 0;
	virtual T BroadcastPrintf(char const*, ...) = 0;
	virtual T SetMaxClients(int) = 0;
	virtual T WriteDeltaEntities(CBaseClient*, CClientFrame*, CClientFrame*, bf_write&) = 0;
	virtual T WriteTempEntities(CBaseClient*, CFrameSnapshot*, CFrameSnapshot*, bf_write&, int) = 0;
	virtual T Init(bool) = 0;
	virtual T Clear() = 0;
	virtual T Shutdown() = 0;
	virtual T CreateFakeClient(char const*) = 0;
	virtual T RemoveClientFromGame(CBaseClient*) = 0;
	virtual T SendClientMessages(bool) = 0;
	virtual T FillServerInfo(SVC_ServerInfo&) = 0;
	virtual T UserInfoChanged(int) = 0;
	virtual T RejectConnection(netadr_s const&, int, const char*) = 0;
	virtual bool CheckIPRestrictions(netadr_s const&, int) = 0;
	virtual void* ConnectClient(netadr_s&, int, int, int, int, char const*, char const*, char const*, int) = 0;
	virtual T GetFreeClient(netadr_s&) = 0;
	virtual T CreateNewClient(int) = 0;
	virtual T FinishCertificateCheck(netadr_s&, int, char const*) = 0;
	virtual int GetChallengeNr(netadr_s&) = 0;
	virtual int GetChallengeType(netadr_s&) = 0;
	virtual bool CheckProtocol(netadr_s&, int) = 0;
	virtual bool CheckChallengeNr(netadr_s&, int) = 0;
	virtual bool CheckChallengeType(CBaseClient*, int, netadr_s&, int, char const*, int) = 0;
	virtual bool CheckPassword(netadr_s&, char const*, char const*) = 0;
	virtual bool CheckIPConnectionReuse(netadr_s&) = 0;
	virtual T ReplyChallenge(netadr_s&) = 0;
	virtual T ReplyServerChallenge(netadr_s&) = 0;
	virtual T CalculateCPUUsage() = 0;
	virtual bool ShouldUpdateMasterServer() = 0;
	virtual T UpdateMasterServerPlayers() = 0;
};