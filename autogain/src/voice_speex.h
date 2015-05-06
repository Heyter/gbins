#ifndef VOICE_SPEEX_H
#define VOICE_SPEEX_H

#include "voice.h"
#include <speex.h>

class Voice_Speex : public Voice
{
public:
	~Voice_Speex();

	bool Init(int quality = 4);

private:
	void DecodeFrame(const char *pCompressed, char *pDecompressedBytes);
	void EncodeFrame(const char *pUncompressedBytes, char *pCompressed);
	bool InitStates();
	void ResetState();
	void TermStates();

private:
	SpeexBits m_Bits;
};

#endif // VOICE_SPEEX_H
