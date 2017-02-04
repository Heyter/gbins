#ifndef WEED_H_ONCE
#define WEED_H_ONCE

#pragma once
#define VTABLE_OFFSET 1

#include <dlfcn.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>

#include "memutils.h"

//#include "vfnhook.h"

#include <tier1/tier1.h>
#include <tier1/convar.h>

#include <interface.h>
#include <netadr.h>
#include <inetmsghandler.h>
#include <inetchannel.h>
#include <steamclientpublic.h>
#include <vstdlib/cvar.h>

/*
typedef enum EAuthSessionResponse
{
	k_EAuthSessionResponseOK = 0,							// Steam has verified the user is online, the ticket is valid and ticket has not been reused.
	k_EAuthSessionResponseUserNotConnectedToSteam = 1,		// The user in question is not connected to steam
	k_EAuthSessionResponseNoLicenseOrExpired = 2,			// The license has expired.
	k_EAuthSessionResponseVACBanned = 3,					// The user is VAC banned for this game.
	k_EAuthSessionResponseLoggedInElseWhere = 4,			// The user account has logged in elsewhere and the session containing the game instance has been disconnected.
	k_EAuthSessionResponseVACCheckTimedOut = 5,				// VAC has been unable to perform anti-cheat checks on this user
	k_EAuthSessionResponseAuthTicketCanceled = 6,			// The ticket has been canceled by the issuer
	k_EAuthSessionResponseAuthTicketInvalidAlreadyUsed = 7,	// This ticket has already been used, it is not valid.
	k_EAuthSessionResponseAuthTicketInvalid = 8,			// This ticket is not from a user instance currently connected to steam.
} EAuthSessionResponse;
*/

struct ValidateAuthTicketResponse_t
{
	enum { k_iCallback = 100 + 43 };

	CSteamID m_SteamID;
	EAuthSessionResponse m_eAuthSessionResponse;
#ifdef NEW_STEAMWORKS
	CSteamID m_OwnerSteamID;
#endif
};

#undef min
#undef max
#include "detours.h"

class CBaseServer;
class CBaseClient;
class CSteam3Server;
class IClient;
class IServer;

#include "tmpclient.h"
#include "tmpserver.h"

#include <GarrysMod/Lua/Interface.h>

extern "C" {
	#include "lua.h"
	
	#include "lualib.h"
	#include "lauxlib.h"
}


#include "hookcall.h"

const char * RenderSteamID(const CSteamID * sid);

#endif