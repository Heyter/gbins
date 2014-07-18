#ifndef VFS_CRC_H
#define VFS_CRC_H

namespace CRC32
{

unsigned int CRC32Hash( unsigned int nSeed, const unsigned char *pData, unsigned int cubData );

void CRC32Init( );

}

#endif // VFS_CRC_H