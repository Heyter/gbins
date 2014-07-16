#include "CRC.h"

namespace CRC32
{

static unsigned int s_DefaultCRCTable[256];
static unsigned int s_DefaultCRCPoly = 0xEDB88320;

unsigned int CRC32Table( unsigned int nPolynomial, unsigned int *pCRCTable )
{
	register unsigned int CRC = 0;
	register unsigned int i = 0;
	register unsigned int j = 0;

	for( i = 0; i < 256; ++i )
	{
		CRC = i;
		for( j = 8; j > 0; --j )
		{
			if( CRC & 1 )
				CRC = ( CRC >> 1 ) ^ nPolynomial;
			else
				CRC >>= 1;
		}

		pCRCTable[i] = CRC;
	}

	return nPolynomial;
}

unsigned int CRC32Hash( unsigned int nSeed, const unsigned char *pData, unsigned int cubData )
{
	register unsigned int CRC = nSeed;
	register unsigned int i = 0;

	for( i = 0; i < cubData; ++i )
		CRC = ( CRC >> 8 ) ^ s_DefaultCRCTable[pData[i] ^ ( CRC & 0x000000FF )];

	return CRC;
}

void CRC32Init( )
{
	CRC32Table( s_DefaultCRCPoly, s_DefaultCRCTable );
}

}