#ifndef _INCLUDE_METAMOD_SOURCE_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_PLUGIN_H_


#ifndef ARRAYSIZE
#define ARRAYSIZE(x)	((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#endif

#ifdef _WIN32
typedef __int64		int64;
#else
typedef long long	int64;
#endif

#define MAX_PLAYERS		129
#define MAX_SCALE		12.0f	// Ignore sample when being scaled over this amount.
#define IDEAL_GAIN		4000.0f	// 10 ^ (72 dB / 20)
#define SAMPLESPERSEC	11025

class IClient;
class Voice_Silk;
class Voice_Speex;
class CDetour;

class AutoGain
{
public:
	AutoGain();

public: // ISmmPlugin
	virtual bool Load();
	virtual bool Unload();

public:
	void OnBroadcastVoiceData(IClient *pClient, int nBytes, char *data);
	void NormalizeClientVoice(int client, char *pVoiceData, int nSamples);

	Voice_Silk *m_Silk[MAX_PLAYERS];
	Voice_Speex *m_Speex[MAX_PLAYERS];
};

extern AutoGain g_AutoGain;


#endif //_INCLUDE_METAMOD_SOURCE_PLUGIN_H_
