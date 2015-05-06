#include <stdlib.h>
#include <string.h>

#include "autogain_mm.h"
#include "voice_silk.h"
#include "voice_speex.h"
#include "MemoryUtils.h"
#include "CDetour/detours.h"

AutoGain g_AutoGain;
PLUGIN_EXPOSE(AutoGain, g_AutoGain);

SH_DECL_MANUALHOOK0(GetPlayerSlot, 0, 0, 0, int); // IClient::GetPlayerSlot

DETOUR_DECL_STATIC4(SV_BroadcastVoiceData, void, IClient *, pClient, int, nBytes, char *, data, int64, xuid)
{
	g_AutoGain.OnBroadcastVoiceData(pClient, nBytes, data);

	DETOUR_STATIC_CALL(SV_BroadcastVoiceData)(pClient, nBytes, data, xuid);
}

#ifdef _WIN32
DETOUR_DECL_STATIC2(SV_BroadcastVoiceData_LTCG, void, char *, data, int64, xuid)
{
	IClient *pClient = NULL;
	int nBytes = 0;

	__asm mov pClient, ecx;
	__asm mov nBytes, edx;

	g_AutoGain.OnBroadcastVoiceData(pClient, nBytes, data);

	__asm mov ecx, pClient;
	__asm mov edx, nBytes;

	DETOUR_STATIC_CALL(SV_BroadcastVoiceData_LTCG)(data, xuid);
}
#endif

AutoGain::AutoGain()
{
	m_VoiceDetour = NULL;

	for (int i = 0; i < ARRAYSIZE(m_Silk); i++)
		m_Silk[i] = NULL;

	for (int i = 0; i < ARRAYSIZE(m_Speex); i++)
		m_Speex[i] = NULL;
}

bool AutoGain::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	// Setup engine-specific data.
	void *engineFactory = (void *)g_SMAPI->GetEngineFactory(false);
	int engineVersion = g_SMAPI->GetSourceEngineBuild();
	void *adrVoiceData = NULL;
	int offsPlayerSlot = 0;

	switch (engineVersion)
	{
		case SOURCE_ENGINE_CSGO:
#ifdef _WIN32
			adrVoiceData = g_MemUtils.FindPattern(engineFactory, "\x55\x8B\xEC\x81\xEC\xD0\x00\x00\x00\x53\x56\x57", 12);
			offsPlayerSlot = 15;
#else
			adrVoiceData = g_MemUtils.ResolveSymbol(engineFactory, "_Z21SV_BroadcastVoiceDataP7IClientiPcx");
			offsPlayerSlot = 16;
#endif
			break;

		case SOURCE_ENGINE_LEFT4DEAD2:
#ifdef _WIN32
			adrVoiceData = g_MemUtils.FindPattern(engineFactory, "\x55\x8B\xEC\x83\xEC\x70\xA1\x2A\x2A\x2A\x2A\x33\xC5\x89\x45\xFC\xA1\x2A\x2A\x2A\x2A\x53\x56", 23);
			offsPlayerSlot = 14;
#else
			adrVoiceData = g_MemUtils.ResolveSymbol(engineFactory, "_Z21SV_BroadcastVoiceDataP7IClientiPcx");
			offsPlayerSlot = 15;
#endif
			break;

		case SOURCE_ENGINE_NUCLEARDAWN:
#ifdef _WIN32
			adrVoiceData = g_MemUtils.FindPattern(engineFactory, "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x83\xEC\x58\x57\x33\xFF", 14);
			offsPlayerSlot = 14;
#else
			adrVoiceData = g_MemUtils.ResolveSymbol(engineFactory, "_Z21SV_BroadcastVoiceDataP7IClientiPcx");
			offsPlayerSlot = 15;
#endif
			break;

		case SOURCE_ENGINE_INSURGENCY:
#ifdef _WIN32
			adrVoiceData = g_MemUtils.FindPattern(engineFactory, "\x55\x8B\xEC\x83\xEC\x74\x68\x2A\x2A\x2A\x2A\x8D\x4D\xE4\xE8", 15);
			offsPlayerSlot = 14;
#else
			adrVoiceData = g_MemUtils.ResolveSymbol(engineFactory, "_Z21SV_BroadcastVoiceDataP7IClientiPcx");
			offsPlayerSlot = 15;
#endif
			break;

		case SOURCE_ENGINE_TF2:
		case SOURCE_ENGINE_CSS:
		case SOURCE_ENGINE_HL2DM:
		case SOURCE_ENGINE_DODS:
		case SOURCE_ENGINE_SDK2013:
#ifdef _WIN32
			adrVoiceData = g_MemUtils.FindPattern(engineFactory, "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x83\xEC\x50\x83\x78\x30", 14);
			offsPlayerSlot = 14;
#else
			adrVoiceData = g_MemUtils.ResolveSymbol(engineFactory, "_Z21SV_BroadcastVoiceDataP7IClientiPcx");
			offsPlayerSlot = 15;
#endif
			break;

		default:
			g_SMAPI->Format(error, maxlen, "Unsupported game.");
			return false;
	}

	if (!adrVoiceData)
	{
		g_SMAPI->Format(error, maxlen, "SV_BroadcastVoiceData sigscan failed.");
		return false;
	}

	SH_MANUALHOOK_RECONFIGURE(GetPlayerSlot, offsPlayerSlot, 0, 0);

	// Setup silk for the first client to ensure it loads.
	m_Silk[0] = new Voice_Silk;

	if (!m_Silk[0]->Init())
	{
		g_SMAPI->Format(error, maxlen, "Failed to init silk codec.");
		return false;
	}

	// Setup speex for the first client to ensure it loads.
	m_Speex[0] = new Voice_Speex;

	if (!m_Speex[0]->Init())
	{
		g_SMAPI->Format(error, maxlen, "Failed to init speex codec.");
		return false;
	}

	// Setup voice detour.
#ifdef _WIN32
	if (engineVersion == SOURCE_ENGINE_CSGO || engineVersion == SOURCE_ENGINE_INSURGENCY)
	{
		m_VoiceDetour = DETOUR_CREATE_STATIC(SV_BroadcastVoiceData_LTCG, adrVoiceData);
	}
	else
	{
		m_VoiceDetour = DETOUR_CREATE_STATIC(SV_BroadcastVoiceData, adrVoiceData);
	}
#else
	m_VoiceDetour = DETOUR_CREATE_STATIC(SV_BroadcastVoiceData, adrVoiceData);
#endif

	if (!m_VoiceDetour)
	{
		g_SMAPI->Format(error, maxlen, "SV_BroadcastVoiceData detour failed.");
		return false;
	}

	m_VoiceDetour->EnableDetour();
	
	return true;
}

bool AutoGain::Unload(char *error, size_t maxlen)
{
	if (m_VoiceDetour)
	{
		m_VoiceDetour->Destroy();
		m_VoiceDetour = NULL;
	}
	
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
	
	int playerslot = SH_MCALL(pClient, GetPlayerSlot)();
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
		pVoiceCodec->Compress(decompressed, nDecompressed, pVoiceData, nVoiceBytes);
	}
}

void AutoGain::NormalizeClientVoice(int client, char *pVoiceData, int nSamples)
{
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

	// Save the previous scale to gradually increment over the buffer.
	static float prevscale[MAX_PLAYERS];

	float targetscale = IDEAL_GAIN / maxgain;
	float scale = prevscale[client];

	// Samples can scale incredibly high as the voice lowers between words.
	if (targetscale > MAX_SCALE)
		targetscale = scale;

	float increment = (targetscale - scale) / nSamples;

	// Regain volume over 2 seconds.
	if (targetscale >= 1.0f)
		increment *= (float)nSamples / (SAMPLESPERSEC * 2);

	for (int i = 0; i < samplemul; i += 2)
	{
		short sample = ((short)pVoiceData[i+1] << 8) | (pVoiceData[i] & 0xFF);

		sample = (short)(sample * scale);
		scale += increment;

		pVoiceData[i+1] = (sample >> 8) & 0xFF;
		pVoiceData[i] = sample & 0xFF;
	}

	prevscale[client] = scale;
}

// Overload a few things to prevent libstdc++ linking.
// Required because we're not linking an HL2SDK.
#if defined __linux__ || defined __APPLE__
extern "C" void __cxa_pure_virtual(void)
{
}

void *operator new(size_t size)
{
	return malloc(size);
}

void *operator new[](size_t size)
{
	return malloc(size);
}

void operator delete(void *ptr)
{
	free(ptr);
}

void operator delete[](void * ptr)
{
	free(ptr);
}
#endif
