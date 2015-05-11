#include <cstdlib>
#include <cstring>

#include <cstdio>

#include "detours.h"
#include "main.h"
#include "voice_silk.h"
#include "voice_speex.h"
#include <eiface.h>
#include "memutils.h"


   FILE * ASD;



AutoGain g_AutoGain;

typedef void (* tBroadcastVoiceData ) ( IClient * , int , char * , int64  ) ;
tBroadcastVoiceData original_BroadcastVoiceData = NULL;
MologieDetours::Detour<tBroadcastVoiceData>* detour_BroadcastVoiceData = NULL;
void hook_BroadcastVoiceData(IClient * pClient, int nBytes, char * data, int64 xuid)
{
	g_AutoGain.OnBroadcastVoiceData(pClient, nBytes, data);
	detour_BroadcastVoiceData->GetOriginalFunction()(pClient, nBytes, data, xuid);
}


AutoGain::AutoGain()
{

	for (int i = 0; i < ARRAYSIZE(m_Silk); i++)
		m_Silk[i] = NULL;

	for (int i = 0; i < ARRAYSIZE(m_Speex); i++)
		m_Speex[i] = NULL;
}

bool AutoGain::Load()
{
	
	
	ASD = fopen ("file.dat", "w+");
	
	void *adrVoiceData = NULL;
	int offsPlayerSlot = 0;

	void *lHandle = dlopen( ENGINE_LIB, RTLD_LAZY  );

	CreateInterfaceFn engineFactory = Sys_GetFactory(ENGINE_LIB);

//#ifdef _WIN32
//	adrVoiceData = g_MemUtils.FindPattern(engineFactory, "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x83\xEC\x50\x83\x78\x30", 14);
//	offsPlayerSlot = 14;
//#else
//	adrVoiceData = ResolveSymbol(engineFactory, "_Z21SV_BroadcastVoiceDataP7IClientiPcx");
//	offsPlayerSlot = 15;
//#endif

	if (!lHandle) return false;
	
	original_BroadcastVoiceData = (tBroadcastVoiceData)ResolveSymbol( lHandle, "_Z21SV_BroadcastVoiceDataP7IClientiPcx" );
	if (original_BroadcastVoiceData) {
			try {
					detour_BroadcastVoiceData = new MologieDetours::Detour<tBroadcastVoiceData>(original_BroadcastVoiceData, hook_BroadcastVoiceData);
			}
			catch(MologieDetours::DetourException &e) {
					Warning("BroadcastVoiceData: Detour failed: Internal error?\n");
			}
	} else {
			Warning("BroadcastVoiceData: Detour failed: Signature not found. (plugin needs updating)\n");
	}



	fGetPlayerSlot = (tGetPlayerSlot)ResolveSymbol(lHandle, "_ZNK11CBaseClient13GetPlayerSlotEv");
	if (!fGetPlayerSlot) 
	{
		Warning("fGetPlayerSlot: Scanning failed: Signature not found.\n");
		return false;
	}


	m_Silk[0] = new Voice_Silk;

	if (!m_Silk[0]->Init())
	{
		return false;
	}

	// Setup speex for the first client to ensure it loads.
	m_Speex[0] = new Voice_Speex;

	if (!m_Speex[0]->Init())
	{
		return false;
	}


	//m_VoiceDetour = DETOUR_CREATE_STATIC(SV_BroadcastVoiceData, adrVoiceData);


	
	return true;
}

bool AutoGain::Unload()
{

	
	for (int i = 0; i < ARRAYSIZE(m_Silk); i++)
	{
		if (m_Silk[i])
		{
			m_Silk[i]->Release();
			m_Silk[i] = NULL;
		}
	}

	for (int i = 0; i < ARRAYSIZE(m_Speex); i++)
	{
		if (m_Speex[i])
		{
			m_Speex[i]->Release();
			m_Speex[i] = NULL;
		}
	}

	return true;
}

void AutoGain::OnBroadcastVoiceData(IClient *pClient, int nBytes, char *data)
{
	if (!pClient || !nBytes || !data)
		return;
	
	int playerslot = fGetPlayerSlot(pClient);
	int nVoiceBytes = nBytes;
	char *pVoiceData = data;
	Voice *pVoiceCodec = NULL;
	
	// Ideally we would check the sv_use_steam_voice cvar here.
	// Instead we'll look for steam headers.
	if (nBytes >= 15)
	{
		unsigned int flags = ((data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7]);
		
		// Check for silk data.
		if (flags == 0x1001001)
		{
			static const unsigned int headerSize = 11;
			unsigned char nPayLoad = data[8];

			if (nPayLoad != 11)
				return; // Not the data we're looking for.

			// Skip the steam header.
			nVoiceBytes = nBytes - headerSize;
			pVoiceData = &data[headerSize];

			// Make sure this player has a codec loaded.
			if (!m_Silk[playerslot])
			{
				m_Silk[playerslot] = new Voice_Silk;
				m_Silk[playerslot]->Init();
			}

			pVoiceCodec = m_Silk[playerslot];
		}
	}

	// Default to speex.
	if (!pVoiceCodec)
	{
		// Make sure this player has a codec loaded.
		if (!m_Speex[playerslot])
		{
			m_Speex[playerslot] = new Voice_Speex;
			m_Speex[playerslot]->Init();
		}

		pVoiceCodec = m_Speex[playerslot];
	}

	// Decompress voice data.
	static char decompressed[8192];
	int nDecompressed = pVoiceCodec->Decompress(pVoiceData, nVoiceBytes, decompressed, sizeof(decompressed));

	if (nDecompressed)
	{
		// Normalize voice volume.
		NormalizeClientVoice(playerslot, decompressed, nDecompressed);

		// Recompress back into the game.
		// pVoiceCodec->Compress(decompressed, nDecompressed, pVoiceData, nVoiceBytes);
	}
}


void AutoGain::NormalizeClientVoice(int client, char *pVoiceData, int nSamples)
{
	
	fwrite(pVoiceData,1,nSamples,ASD);
	
	int samplemul = nSamples * 2;
	int maxgain = 0;

	for (int i = 0; i < samplemul; i += 2)
	{
		short sample = ((short)pVoiceData[i+1] << 8) | (pVoiceData[i] & 0xFF);
		sample = abs(sample);

		if (sample > maxgain)
			maxgain = sample;
	}

	if (maxgain == 0)
		return;
	Warning("%d %d\n",maxgain,nSamples);
}

extern "C" __attribute__( ( visibility("default") ) ) int gmod13_open( lua_State* LL )
{
	g_AutoGain.Load();
	return 0;
}
extern "C" __attribute__( ( visibility("default") ) ) int gmod13_close( lua_State* LL )
{
	g_AutoGain.Unload();
	return 0;
}

