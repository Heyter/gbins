#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "voice_silk.h"
#include "voice_speex.h"
#include <eiface.h>
#include "memutils.h"


AutoGain g_AutoGain;
void SV_BroadcastVoiceData(IClient * pClient, int nBytes, char * data, int64 xuid)
{
	g_AutoGain.OnBroadcastVoiceData(pClient, nBytes, data);

	//(pClient, nBytes, data, xuid);
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
	void *adrVoiceData = NULL;
	int offsPlayerSlot = 0;


    CreateInterfaceFn engineFactory = Sys_GetFactory(ENGINE_LIB);

#ifdef _WIN32
	adrVoiceData = g_MemUtils.FindPattern(engineFactory, "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x83\xEC\x50\x83\x78\x30", 14);
	offsPlayerSlot = 14;
#else
	adrVoiceData = ResolveSymbol(engineFactory, "_Z21SV_BroadcastVoiceDataP7IClientiPcx");
	offsPlayerSlot = 15;
#endif

	fGetPlayerSlot = (tGetPlayerSlot)ResolveSymbol(engineFactory, "_ZNK11CBaseClient13GetPlayerSlotEv");
	
	if (!adrVoiceData)
	{
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

		// Recompress back into the game.
		pVoiceCodec->Compress(decompressed, nDecompressed, pVoiceData, nVoiceBytes);
	}
}
