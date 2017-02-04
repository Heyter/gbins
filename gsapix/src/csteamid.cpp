#include "main.h"

const char * RenderSteamID(const CSteamID * sid)
{
	static char szSteamID[64];
	switch(sid->GetEAccountType())
	{
	case k_EAccountTypeInvalid:
	case k_EAccountTypeIndividual:
		snprintf(szSteamID, sizeof(szSteamID), "STEAM_0:%u:%u", (sid->GetAccountID() % 2) ? 1 : 0, (int32)sid->GetAccountID()/2);
		break;
	default:
		snprintf(szSteamID, sizeof(szSteamID), "%llu", sid->ConvertToUint64());
	}
	return szSteamID;
}