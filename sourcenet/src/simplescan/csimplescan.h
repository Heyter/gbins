#ifndef CSIMPLESCAN_H
#define CSIMPLESCAN_H

#include <interface.h>

#include "sigscan.h"

class CSimpleScan
{
public:
	CSimpleScan();
	CSimpleScan(CreateInterfaceFn iface);
	CSimpleScan(const char *filename);

	bool SetInterface(CreateInterfaceFn iface);
	bool SetDLL(const char *filename);
	
	bool Find(const char *sig, const char *mask, void **func);
private:
	bool m_bInterfaceSet;

	CreateInterfaceFn m_Interface;
	CSigScan m_Signature;
};

#endif //CSIMPLESCAN_H