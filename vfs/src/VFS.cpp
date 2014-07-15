#define NO_SDK

#include "FS.h"
#include "CRC.h"

// GMod
#include "GMLuaModule.h"

#include <vector>
#include <string>

GMOD_MODULE(Startup, Cleanup);

namespace VFS {

static std::vector<FileHandle_t> s_vecHandles;
static std::vector<std::string> s_vecDisallowed;

static bool IsValidFileHandle(FileHandle_t fh)
{
	for(std::vector<FileHandle_t>::iterator itr = s_vecHandles.begin();
		itr != s_vecHandles.end();
		itr++)
	{
		if( *itr == fh )
			return true;
	}
	return false;
}

static bool IsExtensionDisallowed(const char* pszExt)
{
	if( !pszExt )
		return false;

	for(std::vector<std::string>::iterator itr = s_vecDisallowed.begin();
		itr != s_vecDisallowed.end();
		itr++)
	{
		if( *itr == pszExt )
			return true;
	}
	return false;
}

static void FixSlashes(char* pszPath)
{
	while( *pszPath )
	{
		if( *pszPath == '\\' )
			*pszPath = '/';

		pszPath++;
	}
}

static int Open(lua_State* L)
{
	Lua()->CheckType(1, GLua::TYPE_STRING);
	Lua()->CheckType(2, GLua::TYPE_STRING);

	char* pszPath = (char*)Lua()->GetString(1);
	const char* pszMode = Lua()->GetString(2);

	FixSlashes(pszPath);	// I know, I know, I shouldn't be writing to it..

	if( !FS::g_pFilesystem )
	{
		Lua()->Error("Filesystem not initialized");
		return 0;
	}

	// Path validation.
#if defined(WIN32)
	if( pszPath[1] == ':' )
	{
		Lua()->ErrorNoHalt("Invalid Path ( Absolute path not allowed )");
		Lua()->PushNil();
	}
#else
	if( pszPath[1] == "/" )
	{
		Lua()->Error("Invalid Path ( Absolute path not allowed )");
		Lua()->PushNil();
	}
#endif
	else if( strstr(pszPath, "../") != NULL )
	{
		Lua()->ErrorNoHalt("Invalid Path ( You can not leave the directory )");
		Lua()->PushNil();
	}
	else if( IsExtensionDisallowed(strrchr(pszPath, '.')) )
	{
		Lua()->ErrorNoHalt("Extension disallowed");
		Lua()->PushNil();
	}
	else
	{
		char pszVFSPath[1024] = {0};
		sprintf(pszVFSPath, "%s/%s", FILESYSTEM_JAIL_PATH, pszPath);

		char* pszDir = pszVFSPath;

		while( *pszDir )
		{
			if( *pszDir == '/' )
			{
				char cTemp = *pszDir;
				*pszDir = '\0';

				FS::g_pFilesystem->CreateDirHierarchy(pszVFSPath, "GAME");

				*pszDir = cTemp;
			}
			pszDir++;
		}

		FileHandle_t fh = FS::g_pFilesystem->Open(pszVFSPath, pszMode, "GAME");
		if( fh != NULL )
		{
			s_vecHandles.push_back(fh);
			Lua()->PushLightUserData(fh);
		}
		else
		{
			Lua()->PushNil();
		}
	}

	return 1;
}

static int Close(lua_State* L)
{
	Lua()->CheckType(1, GLua::TYPE_LIGHTUSERDATA);

	FileHandle_t fh = (FileHandle_t)Lua()->GetLightUserData(1);
	if( !FS::g_pFilesystem )
	{
		Lua()->Error("Filesystem not initialized.");
		return 0;
	}

	if( !IsValidFileHandle(fh) )
	{
		Lua()->Error("Invalid handle value");
		return 0;
	}

	for(std::vector<FileHandle_t>::iterator itr = s_vecHandles.begin();
		itr != s_vecHandles.end();
		itr++)
	{
		if( *itr == fh )
		{
			s_vecHandles.erase(itr);
			break;
		}
	}

	FS::g_pFilesystem->Close(fh);
	
	return 0;
}

static int Write(lua_State* L)
{
	Lua()->CheckType(1, GLua::TYPE_LIGHTUSERDATA);
	Lua()->CheckType(2, GLua::TYPE_NUMBER);

	FileHandle_t fh = (FileHandle_t)Lua()->GetLightUserData(1);
	unsigned int nWriteType = Lua()->GetInteger(2);

	if( !FS::g_pFilesystem )
	{
		Lua()->Error("Filesystem not initialized.");
		return 0;
	}

	if( !IsValidFileHandle(fh) )
	{
		Lua()->Error("Invalid handle value");
		return 0;
	}

	int nWrite = 0;

	switch(nWriteType)
	{
	case VFS_READWRITE_DATA:
		{
			Lua()->CheckType(3, GLua::TYPE_STRING);
			
			const char* pData = Lua()->GetString(3);
			unsigned int cubDataMax = Lua()->StringLength(3);
			unsigned int cubData = cubDataMax;
			unsigned int nTop = Lua()->GetStackTop();

			if( nTop >= 4 )
			{
				Lua()->CheckType(4, GLua::TYPE_NUMBER);
				cubData = Lua()->GetInteger(4);
			}
			
			if( cubData > cubDataMax )
				cubData = cubDataMax;

			nWrite = FS::g_pFilesystem->Write(pData, cubData, fh);
		}
		break;
	case VFS_READWRITE_STRING:
		{
			Lua()->CheckType(3, GLua::TYPE_STRING);

			const char* pData = Lua()->GetString(3);

			nWrite = FS::g_pFilesystem->Write(pData, strlen(pData) + 1, fh);
		}
		break;
	case VFS_READWRITE_UINT32:
		{
			Lua()->CheckType(3, GLua::TYPE_NUMBER);

			unsigned int nData = Lua()->GetInteger(3);

			nWrite = FS::g_pFilesystem->Write(&nData, sizeof(unsigned int), fh);
		}
		break;
	case VFS_READWRITE_UINT16:
		{
			Lua()->CheckType(3, GLua::TYPE_NUMBER);

			unsigned short nData = (unsigned short)Lua()->GetInteger(3);

			nWrite = FS::g_pFilesystem->Write(&nData, sizeof(unsigned short), fh);
		}
		break;
	case VFS_READWRITE_BYTE:
	case VFS_READWRITE_UINT8:
		{
			Lua()->CheckType(3, GLua::TYPE_NUMBER);

			unsigned char nData = (unsigned char)Lua()->GetInteger(3);

			nWrite = FS::g_pFilesystem->Write(&nData, sizeof(unsigned char), fh);
		}
		break;
	case VFS_READWRITE_DOUBLE:
		{
			Lua()->CheckType(3, GLua::TYPE_NUMBER);

			double dbData = Lua()->GetDouble(3);

			nWrite = FS::g_pFilesystem->Write(&dbData, sizeof(double), fh);
		}
		break;
	case VFS_READWRITE_FLOAT:
		{
			Lua()->CheckType(3, GLua::TYPE_NUMBER);

			float flData = Lua()->GetNumber(3);

			nWrite = FS::g_pFilesystem->Write(&flData, sizeof(float), fh);
		}
		break;
	default:
		{
			Lua()->Error("Unsupported write method.");
		}
		break;
	}

	Lua()->PushLong(nWrite);

	return 1;
}

static int Read(lua_State* L)
{
	Lua()->CheckType(1, GLua::TYPE_LIGHTUSERDATA);
	Lua()->CheckType(2, GLua::TYPE_NUMBER);

	FileHandle_t fh = (FileHandle_t)Lua()->GetLightUserData(1);
	unsigned int nReadType = Lua()->GetInteger(2);

	if( !FS::g_pFilesystem )
	{
		Lua()->Error("Filesystem not initialized.");
		return 0;
	}

	if( !IsValidFileHandle(fh) )
	{
		Lua()->Error("Invalid handle value");
		return 0;
	}

	int nRead = 0;

	switch(nReadType)
	{
	case VFS_READWRITE_STRING:
		{
			unsigned char cTemp;
			std::string str;

			while( !FS::g_pFilesystem->EndOfFile(fh) )
			{
				if( FS::g_pFilesystem->Read(&cTemp, 1, fh) == 0 )
					break;

				nRead++;

				if( cTemp == 0 )
					break;

				str += cTemp;
			}

			Lua()->Push(str.c_str());
		}
		break;
	case VFS_READWRITE_UINT32:
		{
			unsigned int nData = 0;

			nRead = FS::g_pFilesystem->Read(&nData, sizeof(unsigned int), fh);
			if( nRead == 0 )
				Lua()->PushNil();
			else
				Lua()->PushLong(nData);
		}
		break;
	case VFS_READWRITE_UINT16:
		{
			unsigned short nData = 0;

			nRead = FS::g_pFilesystem->Read(&nData, sizeof(unsigned short), fh);

			if( nRead == 0 )
				Lua()->PushNil();
			else
				Lua()->PushLong(nData);
		}
		break;
	case VFS_READWRITE_BYTE:
	case VFS_READWRITE_UINT8:
		{
			unsigned char nData = 0;

			nRead = FS::g_pFilesystem->Read(&nData, sizeof(unsigned char), fh);

			if( nRead == 0 )
				Lua()->PushNil();
			else
				Lua()->PushLong(nData);
		}
		break;
	case VFS_READWRITE_DOUBLE:
		{
			double dbData = 0;

			nRead = FS::g_pFilesystem->Read(&dbData, sizeof(double), fh);

			if( nRead == 0 )
				Lua()->PushNil();
			else
				Lua()->PushDouble(dbData);
		}
		break;
	case VFS_READWRITE_FLOAT:
		{
			float flData = 0;

			nRead = FS::g_pFilesystem->Read(&flData, sizeof(float), fh);

			if( nRead == 0 )
				Lua()->PushNil();
			else
				Lua()->PushDouble(flData);
		}
		break;
	default:
		{
			Lua()->Error("Unsupported read method");
			Lua()->PushNil();
		}
		break;
	}

	return 1;
}

static int Seek(lua_State* L)
{
	Lua()->CheckType(1, GLua::TYPE_LIGHTUSERDATA);
	Lua()->CheckType(2, GLua::TYPE_NUMBER);
	Lua()->CheckType(3, GLua::TYPE_NUMBER);

	FileHandle_t fh = (FileHandle_t)Lua()->GetLightUserData(1);
	int nPos = Lua()->GetInteger(2);
	int nMethod = Lua()->GetInteger(3);

	if( !FS::g_pFilesystem )
	{
		Lua()->Error("Filesystem not initialized");
		return 0;
	}

	if( !IsValidFileHandle(fh) )
	{
		Lua()->Error("Invalid handle value");
		return 0;
	}

	FS::g_pFilesystem->Seek(fh, nPos, (FileSystemSeek_t)nMethod);

	return 0;
}

static int Tell(lua_State* L)
{
	Lua()->CheckType(1, GLua::TYPE_LIGHTUSERDATA);

	FileHandle_t fh = (FileHandle_t)Lua()->GetLightUserData(1);

	if( !FS::g_pFilesystem )
	{
		Lua()->Error("Filesystem not initialized");
		return 0;
	}

	if( !IsValidFileHandle(fh) )
	{
		Lua()->Error("Invalid handle value");
		return 0;
	}

	unsigned int nPos = FS::g_pFilesystem->Tell(fh);
	Lua()->PushLong(nPos);

	return 1;
}

static int IsEOF(lua_State* L)
{
	Lua()->CheckType(1, GLua::TYPE_LIGHTUSERDATA);

	FileHandle_t fh = (FileHandle_t)Lua()->GetLightUserData(1);

	if( !FS::g_pFilesystem )
	{
		Lua()->Error("Filesystem not initialized");
		return 0;
	}

	if( !IsValidFileHandle(fh) )
	{
		Lua()->Error("Invalid handle value");
		return 0;
	}

	bool fEOF = FS::g_pFilesystem->EndOfFile(fh);
	Lua()->Push(fEOF);

	return 1;
}

static int CRC_32(lua_State* L)
{
	Lua()->CheckType(1, GLua::TYPE_LIGHTUSERDATA);

	FileHandle_t fh = (FileHandle_t)Lua()->GetLightUserData(1);

	if( !FS::g_pFilesystem )
	{
		Lua()->Error("Filesystem not initialized");
		return 0;
	}

	if( !IsValidFileHandle(fh) )
	{
		Lua()->Error("Invalid handle value");
		return 0;
	}

	unsigned int nCRC32 = 0xFFFFFFFF;
	unsigned int nCurPos = FS::g_pFilesystem->Tell(fh);
	unsigned char szBuffer[512];

	FS::g_pFilesystem->Seek(fh, 0, FILESYSTEM_SEEK_HEAD);

	while( !FS::g_pFilesystem->EndOfFile(fh) )
	{
		int nRead = FS::g_pFilesystem->Read(szBuffer, 512, fh);
		if( nRead == 0 )
			break;

		nCRC32 = CRC32::CRC32Hash(nCRC32, szBuffer, nRead);
	}
	nCRC32 = ~nCRC32;

	FS::g_pFilesystem->Seek(fh, nCurPos, FILESYSTEM_SEEK_HEAD);

	Lua()->PushDouble((double)nCRC32);

	return 1;
}

}

int Startup(lua_State* L)
{
	FS::LoadFilesystem();
	CRC32::CRC32Init();

	VFS::s_vecDisallowed.push_back(".dll");
	VFS::s_vecDisallowed.push_back(".exe");
	VFS::s_vecDisallowed.push_back(".so");

	ILuaObject* pTable = Lua()->GetNewTable();
	{
		// vfs.Open(file, mode)
		pTable->SetMember("Open", VFS::Open);
		// vfs.Close(file)
		pTable->SetMember("Close", VFS::Close);
		// vfs.Write(handle, write_type, data)
		pTable->SetMember("Write", VFS::Write);
		// vfs.Read(handle, read_type)
		pTable->SetMember("Read", VFS::Read);
		// vfs.Seek(handle, pos, method)
		pTable->SetMember("Seek", VFS::Seek);
		// vfs.Tell(handle)
		pTable->SetMember("Tell", VFS::Tell);
		// vfs.EOF(handle)
		pTable->SetMember("EOF", VFS::IsEOF);
		// vfs.CRC32(handle)
		pTable->SetMember("CRC32", VFS::CRC_32);
	}
	Lua()->SetGlobal("vfs", pTable);
	pTable->UnReference();

	Lua()->SetGlobal("VFS_SEEK_SET", (float)FILESYSTEM_SEEK_HEAD);
	Lua()->SetGlobal("VFS_SEEK_CURRENT", (float)FILESYSTEM_SEEK_CURRENT);
	Lua()->SetGlobal("VFS_SEEK_END", (float)FILESYSTEM_SEEK_TAIL);

	Lua()->SetGlobal("VFS_WRITE_DATA", (float)VFS_READWRITE_DATA);
	Lua()->SetGlobal("VFS_WRITE_STRING", (float)VFS_READWRITE_STRING);
	Lua()->SetGlobal("VFS_WRITE_UINT32", (float)VFS_READWRITE_UINT32);
	Lua()->SetGlobal("VFS_WRITE_UINT16", (float)VFS_READWRITE_UINT16);
	Lua()->SetGlobal("VFS_WRITE_UINT8", (float)VFS_READWRITE_UINT8);
	Lua()->SetGlobal("VFS_WRITE_FLOAT", (float)VFS_READWRITE_FLOAT);
	Lua()->SetGlobal("VFS_WRITE_DOUBLE", (float)VFS_READWRITE_DOUBLE);
	Lua()->SetGlobal("VFS_WRITE_BYTE", (float)VFS_READWRITE_UINT8);

	Lua()->SetGlobal("VFS_READ_STRING", (float)VFS_READWRITE_STRING);
	Lua()->SetGlobal("VFS_READ_UINT32", (float)VFS_READWRITE_UINT32);
	Lua()->SetGlobal("VFS_READ_UINT16", (float)VFS_READWRITE_UINT16);
	Lua()->SetGlobal("VFS_READ_UINT8", (float)VFS_READWRITE_UINT8);
	Lua()->SetGlobal("VFS_READ_FLOAT", (float)VFS_READWRITE_FLOAT);
	Lua()->SetGlobal("VFS_READ_DOUBLE", (float)VFS_READWRITE_DOUBLE);
	Lua()->SetGlobal("VFS_READ_BYTE", (float)VFS_READWRITE_UINT8);

	return 0;
}

int Cleanup(lua_State* L)
{
	for(std::vector<FileHandle_t>::iterator itr = VFS::s_vecHandles.begin();
		itr != VFS::s_vecHandles.end();
		itr++)
	{
		if( FS::g_pFilesystem )
			FS::g_pFilesystem->Close(*itr);
	}

	FS::UnloadFilesystem();
	return 0;
}