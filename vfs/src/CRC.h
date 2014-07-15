#if !defined(VFS_CRC_H)
#define VFS_CRC_H

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/crc.hpp>

namespace CRC32
{

unsigned int CRC32Table(unsigned int nPolynomial, 
	unsigned int CRCTable[256]);

unsigned int CRC32Hash(unsigned int nSeed, 
	const unsigned char* pData, 
	unsigned int cubData);

void CRC32Init(void);

}

#endif // VFS_CRC_H