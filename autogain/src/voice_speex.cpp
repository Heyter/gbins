#include "voice_speex.h"

#include <string.h>

#define SAMPLERATE			8000	// get 8000 samples/sec
#define RAW_FRAME_SIZE		160		// in 160 samples per frame

const int ENCODED_FRAME_SIZE[11] = { 6, 6, 15, 15, 20, 20, 28, 28, 38, 38, 38 };

Voice_Speex::~Voice_Speex()
{
	TermStates();
}

bool Voice_Speex::Init(int quality)
{
	if (!InitStates())
		return false;

	m_nRawBytes = RAW_FRAME_SIZE * BYTES_PER_SAMPLE;
	m_nRawSamples = m_nRawBytes >> 1;

	// map gerneral voice quality 1-5 to speex quality levels
	switch (quality)
	{
		case 1:		m_Quality = 0; break;
		case 2:		m_Quality = 2; break;
		case 3:		m_Quality = 4; break;
		case 4:		m_Quality = 6; break;
		case 5: 	m_Quality = 8; break;
		default:	m_Quality = 0; break;
	}

	m_nEncodedBytes = ENCODED_FRAME_SIZE[m_Quality];

	speex_encoder_ctl(m_EncoderState, SPEEX_SET_QUALITY, &m_Quality);
	speex_decoder_ctl(m_DecoderState, SPEEX_SET_QUALITY, &m_Quality);

	int postfilter = 1; // Set the perceptual enhancement on
	speex_decoder_ctl(m_DecoderState, SPEEX_SET_ENH, &postfilter);

	int samplerate = SAMPLERATE;
	speex_decoder_ctl(m_DecoderState, SPEEX_SET_SAMPLING_RATE, &samplerate);
	speex_encoder_ctl(m_EncoderState, SPEEX_SET_SAMPLING_RATE, &samplerate);

	return true;
}

void Voice_Speex::EncodeFrame(const char *pUncompressedBytes, char *pCompressed)
{
	float input[RAW_FRAME_SIZE];
	short *in = (short*)pUncompressedBytes;

	/*Copy the 16 bits values to float so Speex can work on them*/
	for (int i = 0; i < RAW_FRAME_SIZE; i++)
	{
		input[i] = (float)*in;
		in++;
	}

	/*Flush all the bits in the struct so we can encode a new frame*/
	speex_bits_reset(&m_Bits);

	/*Encode the frame*/
	speex_encode(m_EncoderState, input, &m_Bits);

	/*Copy the bits to an array of char that can be written*/
	speex_bits_write(&m_Bits, pCompressed, ENCODED_FRAME_SIZE[m_Quality]);
}

void Voice_Speex::DecodeFrame(const char *pCompressed, char *pDecompressedBytes)
{
	float output[RAW_FRAME_SIZE];
	short *out = (short*)pDecompressedBytes;

	/*Copy the data into the bit-stream struct*/
	speex_bits_read_from(&m_Bits, (char *)pCompressed, ENCODED_FRAME_SIZE[m_Quality]);

	/*Decode the data*/
	speex_decode(m_DecoderState, &m_Bits, output);

	/*Copy from float to short (16 bits) for output*/
	for (int i = 0; i < RAW_FRAME_SIZE; i++)
	{
		*out = (short)output[i];
		out++;
	}
}

bool Voice_Speex::InitStates()
{
	speex_bits_init(&m_Bits);

	m_EncoderState = speex_encoder_init(&speex_nb_mode);	// narrow band mode 8kbp
	m_DecoderState = speex_decoder_init(&speex_nb_mode);

	return m_EncoderState && m_DecoderState;
}

void Voice_Speex::ResetState()
{
	speex_encoder_ctl(m_EncoderState, SPEEX_RESET_STATE, NULL);
	speex_decoder_ctl(m_DecoderState, SPEEX_RESET_STATE, NULL);
}

void Voice_Speex::TermStates()
{
	if (m_EncoderState)
	{
		speex_encoder_destroy(m_EncoderState);
		m_EncoderState = NULL;
	}

	if (m_DecoderState)
	{
		speex_decoder_destroy(m_DecoderState);
		m_DecoderState = NULL;
	}

	speex_bits_destroy(&m_Bits);
}