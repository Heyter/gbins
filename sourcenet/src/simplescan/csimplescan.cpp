#include "csimplescan.h"

CSimpleScan::CSimpleScan()
{
}

CSimpleScan::CSimpleScan(CreateInterfaceFn iface)
{
	SetInterface(iface);
}

CSimpleScan::CSimpleScan(const char *filename)
{
	SetDLL(filename);
}

bool CSimpleScan::SetInterface(CreateInterfaceFn iface)
{
	if (!iface)
		return false;

	m_Interface = iface;

	CSigScan::sigscan_dllfunc = m_Interface;

	if (!CSigScan::GetDllMemInfo())
		return false;

	return (m_bInterfaceSet = true);
}

bool CSimpleScan::SetDLL(const char *filename)
{
	return SetInterface(Sys_GetFactory(filename));
}

#include <dbg.h>

bool CSimpleScan::Find(const char *sig, const char *mask, void **func)
{
	if (!m_bInterfaceSet)
		return false;

	m_Signature.Init((unsigned char *)sig, (char *)mask, strlen(mask));

	if (!m_Signature.is_set)
		return false;

	*func = m_Signature.sig_addr;

	return true;
}