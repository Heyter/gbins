#define NO_SDK

#include "FS.h"
#include "CRC.h"

#include <GarrysMod/Lua/Interface.h>

#include <vector>
#include <string>

namespace VFS
{

struct UserData
{
	FileHandle_t data;
	unsigned char type;
	bool valid;
};

#define GET_FH( ) ( ( (UserData *)LUA->GetUserdata( 1 ) )->data )

static std::vector<std::string> s_vecDisallowed;

static bool CheckIsValid( UserData *ud )
{
	return ud->valid;
}

static bool IsExtensionDisallowed( const char *pszExt )
{
	if( !pszExt )
		return false;

	for( std::vector<std::string>::iterator itr = s_vecDisallowed.begin( ); itr != s_vecDisallowed.end( ); ++itr )
		if( *itr == pszExt )
			return true;

	return false;
}

static int Open( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::STRING );
	LUA->CheckType( 2, GarrysMod::Lua::Type::STRING );

	unsigned int len = 0;
	const char *pszPath = LUA->GetString( 1, &len );
	const char *pszMode = LUA->GetString( 2 );
	
	if( !FS::g_pFilesystem )
		LUA->ThrowError( "Filesystem not initialized" );

	std::string strPath = pszPath;
	size_t pos = -1;
	while( ( pos = strPath.find( '\\', pos + 1 ) ) != strPath.npos )
		strPath.replace( pos, pos, 1, '/' );

	// Path validation.
#if defined _WIN32
	if( strPath[1] == ':' )
	{
		Msg( "Invalid Path ( Absolute path not allowed )" );
		LUA->PushNil( );
		return 1;
	}
#else
	if( strPath[0] == "/" )
	{
		Msg( "Invalid Path ( Absolute path not allowed )" );
		LUA->PushNil( );
		return 1;
	}
#endif

	if( strPath.find( "../" ) != strPath.npos )
	{
		Msg( "Invalid Path ( You can not leave the directory )" );
		LUA->PushNil( );
		return 1;
	}

	pos = strPath.rfind( '.' );
	if( pos != strPath.npos && IsExtensionDisallowed( &strPath[pos] ) )
	{
		Msg( "Extension disallowed");
		LUA->PushNil( );
		return 1;
	}

	strPath.insert( 0, FILESYSTEM_JAIL_PATH "/" );
	FS::g_pFilesystem->CreateDirHierarchy( strPath.c_str( ), "GAME" );

	FileHandle_t fh = FS::g_pFilesystem->Open( strPath.c_str( ), pszMode, "GAME" );
	if( fh != NULL )
	{
		UserData *userdata = (UserData *)LUA->NewUserdata( sizeof( UserData ) );
		userdata->data = fh;
		userdata->type = GarrysMod::Lua::Type::USERDATA;	
		userdata->valid = true;

		LUA->CreateMetaTableType( "IVFS", GarrysMod::Lua::Type::USERDATA );
		LUA->SetMetaTable( -2 );
	}
	else
	{
		LUA->PushNil( );
	}

	return 1;
}

static int IsValidFH( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::USERDATA );

	UserData *userdata = (UserData *)LUA->GetUserdata( 1 );
	
	if( !FS::g_pFilesystem )
		LUA->ThrowError("Filesystem not initialized.");

	if( CheckIsValid( userdata ) )
	{
		LUA->PushBool( true );
		return 1;
	}
	
	return 0;
}

static int Close( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::USERDATA );

	UserData *userdata = (UserData *)LUA->GetUserdata( 1 );
	FileHandle_t fh = GET_FH( );
	
	if( !FS::g_pFilesystem )
		LUA->ThrowError( "Filesystem not initialized." );

	if( !CheckIsValid( userdata ) )
		LUA->ThrowError( "Invalid handle value" );

	userdata->valid = false;
	FS::g_pFilesystem->Close( fh );
	
	return 0;
}

static int Flush( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::USERDATA );

	UserData *userdata = (UserData *)LUA->GetUserdata( 1 );
	FileHandle_t fh = GET_FH( );

	if( !FS::g_pFilesystem )
		LUA->ThrowError( "Filesystem not initialized." );

	if( !CheckIsValid(userdata) )
		LUA->ThrowError( "Invalid handle value" );

	FS::g_pFilesystem->Flush( fh );
	
	return 0;
}

static int Write( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::USERDATA );
	LUA->CheckType( 2, GarrysMod::Lua::Type::NUMBER );

	UserData *userdata = (UserData *)LUA->GetUserdata( 1 );
	FileHandle_t fh = GET_FH( );

	if( !FS::g_pFilesystem )
		LUA->ThrowError( "Filesystem not initialized." );

	if( !CheckIsValid( userdata ) )
		LUA->ThrowError( "Invalid handle value" );

	int nWrite = 0;
	switch( (FS::ReadWrite)( (int)LUA->GetNumber( 2 ) ) )
	{
		case FS::ReadWrite::data:
		{
			LUA->CheckType( 3, GarrysMod::Lua::Type::STRING );
			
			unsigned int cubDataMax = 0;
			const char *pData = LUA->GetString( 3, &cubDataMax );
			unsigned int cubData = cubDataMax;

			if( LUA->Top( ) >= 4 )
			{
				LUA->CheckType( 4, GarrysMod::Lua::Type::NUMBER );
				cubData = (unsigned int)LUA->GetNumber( 4 );
			}
			
			if( cubData > cubDataMax )
				cubData = cubDataMax;

			nWrite = FS::g_pFilesystem->Write( pData, cubData, fh );
			break;
		}

		case FS::ReadWrite::string:
		{
			LUA->CheckType( 3, GarrysMod::Lua::Type::STRING );
			const char *pData = LUA->GetString( 3 );
			nWrite = FS::g_pFilesystem->Write( pData, strlen( pData ) + 1, fh );
			break;
		}

		case FS::ReadWrite::uint32:
		{
			LUA->CheckType( 3, GarrysMod::Lua::Type::NUMBER );
			unsigned int nData = (unsigned int)LUA->GetNumber( 3 );
			nWrite = FS::g_pFilesystem->Write( &nData, sizeof( nData ), fh );
			break;
		}

		case FS::ReadWrite::uint16:
		{
			LUA->CheckType( 3, GarrysMod::Lua::Type::NUMBER );
			unsigned short nData = (unsigned short)LUA->GetNumber( 3 );
			nWrite = FS::g_pFilesystem->Write( &nData, sizeof( nData ), fh );
			break;
		}

		case FS::ReadWrite::byte:
		case FS::ReadWrite::uint8:
		{
			LUA->CheckType( 3, GarrysMod::Lua::Type::NUMBER );
			unsigned char nData = (unsigned char)LUA->GetNumber( 3 );
			nWrite = FS::g_pFilesystem->Write( &nData, sizeof( nData ), fh );
			break;
		}

		case FS::ReadWrite::doublefp:
		{
			LUA->CheckType( 3, GarrysMod::Lua::Type::NUMBER );
			double dbData = LUA->GetNumber( 3 );
			nWrite = FS::g_pFilesystem->Write( &dbData, sizeof( dbData ), fh );
			break;
		}

		case FS::ReadWrite::fp:
		{
			LUA->CheckType( 3, GarrysMod::Lua::Type::NUMBER );
			float flData = (float)LUA->GetNumber( 3 );
			nWrite = FS::g_pFilesystem->Write( &flData, sizeof( flData ), fh );
			break;
		}

		default:
			LUA->ThrowError( "Unsupported write method." );
			break;
	}

	LUA->PushNumber( nWrite );
	return 1;
}

static int Read( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::USERDATA );
	LUA->CheckType( 2, GarrysMod::Lua::Type::NUMBER );

	UserData *userdata = (UserData *)LUA->GetUserdata( 1 );
	FileHandle_t fh = GET_FH( );

	if( !FS::g_pFilesystem )
		LUA->ThrowError( "Filesystem not initialized." );

	if( !CheckIsValid( userdata ) )
		LUA->ThrowError( "Invalid handle value" );

	switch( (FS::ReadWrite)( (int)LUA->GetNumber( 2 ) ) )
	{
		case FS::ReadWrite::string:
		{
			std::string str;

			unsigned char cTemp = '\0';
			while( FS::g_pFilesystem->Read( &cTemp, sizeof( cTemp ), fh ) == sizeof( cTemp ) )
			{
				if( cTemp == '\0' )
					break;

				str += cTemp;
			}

			LUA->PushString( str.c_str( ) );
			break;
		}

		case FS::ReadWrite::data:
		{
			LUA->CheckType( 3, GarrysMod::Lua::Type::NUMBER );
			unsigned int len = (unsigned int)LUA->GetNumber( 3 );

			std::string str( len, '\0' );
			int nRead = FS::g_pFilesystem->Read( &str[0], len, fh );
			LUA->PushString( str.c_str( ), nRead );

			break;
		}

		case FS::ReadWrite::uint32:
		{
			unsigned int nData = 0;
			if( FS::g_pFilesystem->Read( &nData, sizeof( nData ), fh ) != sizeof( nData ) )
				LUA->PushNil();
			else
				LUA->PushNumber( nData );

			break;
		}

		case FS::ReadWrite::uint16:
		{
			unsigned short nData = 0;
			if( FS::g_pFilesystem->Read( &nData, sizeof( nData ), fh ) != sizeof( nData ) )
				LUA->PushNil( );
			else
				LUA->PushNumber( nData );

			break;
		}

		case FS::ReadWrite::byte:
		case FS::ReadWrite::uint8:
		{
			unsigned char nData = 0;
			if( FS::g_pFilesystem->Read( &nData, sizeof( nData ), fh ) != sizeof( nData ) )
				LUA->PushNil( );
			else
				LUA->PushNumber( nData );

			break;
		}

		case FS::ReadWrite::doublefp:
		{
			double dbData = 0;
			if( FS::g_pFilesystem->Read( &dbData, sizeof( dbData ), fh ) != sizeof( dbData ) )
				LUA->PushNil( );
			else
				LUA->PushNumber( dbData );

			break;
		}

		case FS::ReadWrite::fp:
		{
			float flData = 0;
			if( FS::g_pFilesystem->Read( &flData, sizeof( flData ), fh ) != sizeof( flData ) )
				LUA->PushNil( );
			else
				LUA->PushNumber( flData );

			break;
		}

		default:
			LUA->ThrowError( "Unsupported read method" );
			break;
	}

	return 1;
}

static int Seek( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::USERDATA );
	LUA->CheckType( 2, GarrysMod::Lua::Type::NUMBER );
	LUA->CheckType( 3, GarrysMod::Lua::Type::NUMBER );

	UserData *userdata = (UserData *)LUA->GetUserdata( 1 );
	FileHandle_t fh = GET_FH( );

	int nPos = (int)LUA->GetNumber( 2 );
	int nMethod = (int)LUA->GetNumber( 3 );

	if( !FS::g_pFilesystem )
		LUA->ThrowError( "Filesystem not initialized" );

	if( !CheckIsValid( userdata ) )
		LUA->ThrowError( "Invalid handle value" );

	FS::g_pFilesystem->Seek( fh, nPos, (FileSystemSeek_t)nMethod );

	return 0;
}

static int GarbageCollect( lua_State *state )
{
	if( !LUA->IsType( 1, GarrysMod::Lua::Type::USERDATA ) )
	{
		Warning( "GC: madness\n" );
		return 0;
	}

	UserData *userdata = (UserData *)LUA->GetUserdata( 1 );
	FileHandle_t fh = GET_FH( );

	Warning( "GC: %p %p\n", userdata, fh );

	if( !FS::g_pFilesystem )
	{
		Warning( "GC: Filesystem not initialized\n" );
		return 0;
	}

	if( !CheckIsValid( userdata ) )
	{
		Warning( "GC: Invalid handle value (already closed?)\n" );
		return 0;
	}
	
	userdata->valid = false;
	FS::g_pFilesystem->Close( fh );
	
	return 0;
}

static int Tell( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::USERDATA );

	UserData *userdata = (UserData *)LUA->GetUserdata( 1 );
	FileHandle_t fh = GET_FH( );

	if( !FS::g_pFilesystem )
		LUA->ThrowError( "Filesystem not initialized" );

	if( !CheckIsValid( userdata ) )
		LUA->ThrowError( "Invalid handle value" );

	LUA->PushNumber( FS::g_pFilesystem->Tell( fh ) );
	return 1;
}

static int IsEOF( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::USERDATA );

	UserData *userdata = (UserData *)LUA->GetUserdata( 1 );
	FileHandle_t fh = GET_FH( );

	if( !FS::g_pFilesystem )
		LUA->ThrowError( "Filesystem not initialized" );

	if( !CheckIsValid( userdata ) )
		LUA->ThrowError( "Invalid handle value" );

	LUA->Push( FS::g_pFilesystem->EndOfFile( fh ) );
	return 1;
}

static int CRC32( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::USERDATA );

	UserData *userdata = (UserData *)LUA->GetUserdata( 1 );
	FileHandle_t fh = GET_FH( );

	if( !FS::g_pFilesystem )
		LUA->ThrowError( "Filesystem not initialized" );

	if( !CheckIsValid( userdata ) )
		LUA->ThrowError( "Invalid handle value" );

	unsigned int nCRC32 = 0xFFFFFFFF;
	unsigned int nCurPos = FS::g_pFilesystem->Tell( fh );
	unsigned char szBuffer[512];

	FS::g_pFilesystem->Seek( fh, 0, FILESYSTEM_SEEK_HEAD );

	int nRead = 0;
	while( !FS::g_pFilesystem->EndOfFile( fh ) )
	{
		if( ( nRead = FS::g_pFilesystem->Read( szBuffer, 512, fh ) ) <= 0 )
			break;

		nCRC32 = CRC32::CRC32Hash( nCRC32, szBuffer, nRead );
	}

	FS::g_pFilesystem->Seek( fh, nCurPos, FILESYSTEM_SEEK_HEAD );

	LUA->Push( (double)~nCRC32 );
	return 1;
}

}

GMOD_MODULE_OPEN( )
{
	if( !FS::LoadFilesystem( ) )
		LUA->ThrowError( "Filesystem failed to load" );
	
	CRC32::CRC32Init( );

	VFS::s_vecDisallowed.push_back( ".cfg" );
	VFS::s_vecDisallowed.push_back( ".res" );
	VFS::s_vecDisallowed.push_back( ".rc" );
	VFS::s_vecDisallowed.push_back( ".fgd" );
	VFS::s_vecDisallowed.push_back( ".inf" );
	VFS::s_vecDisallowed.push_back( ".db" );
	VFS::s_vecDisallowed.push_back( ".vfs" );
	VFS::s_vecDisallowed.push_back( ".dt" );
	VFS::s_vecDisallowed.push_back( ".ver" );
	VFS::s_vecDisallowed.push_back( ".vpk" );
	VFS::s_vecDisallowed.push_back( ".sav" );
	VFS::s_vecDisallowed.push_back( ".gma" );
	VFS::s_vecDisallowed.push_back( ".bz2" );
	VFS::s_vecDisallowed.push_back( ".zip" );
	VFS::s_vecDisallowed.push_back( ".rar" );
	
	VFS::s_vecDisallowed.push_back( ".ade" );
	VFS::s_vecDisallowed.push_back( ".adp" );
	VFS::s_vecDisallowed.push_back( ".app" );
	VFS::s_vecDisallowed.push_back( ".asp" );
	VFS::s_vecDisallowed.push_back( ".bas" );
	VFS::s_vecDisallowed.push_back( ".bat" );
	VFS::s_vecDisallowed.push_back( ".cer" );
	VFS::s_vecDisallowed.push_back( ".chm" );
	VFS::s_vecDisallowed.push_back( ".cmd" );
	VFS::s_vecDisallowed.push_back( ".com" );
	VFS::s_vecDisallowed.push_back( ".cpl" );
	VFS::s_vecDisallowed.push_back( ".crt" );
	VFS::s_vecDisallowed.push_back( ".csh" );
	VFS::s_vecDisallowed.push_back( ".der" );
	VFS::s_vecDisallowed.push_back( ".exe" );
	VFS::s_vecDisallowed.push_back( ".fxp" );
	VFS::s_vecDisallowed.push_back( ".gadget" );
	VFS::s_vecDisallowed.push_back( ".hlp" );
	VFS::s_vecDisallowed.push_back( ".hta" );
	VFS::s_vecDisallowed.push_back( ".inf" );
	VFS::s_vecDisallowed.push_back( ".ins" );
	VFS::s_vecDisallowed.push_back( ".isp" );
	VFS::s_vecDisallowed.push_back( ".its" );
	VFS::s_vecDisallowed.push_back( ".js" );
	VFS::s_vecDisallowed.push_back( ".jse" );
	VFS::s_vecDisallowed.push_back( ".ksh" );
	VFS::s_vecDisallowed.push_back( ".lnk" );
	VFS::s_vecDisallowed.push_back( ".mad" );
	VFS::s_vecDisallowed.push_back( ".maf" );
	VFS::s_vecDisallowed.push_back( ".mag" );
	VFS::s_vecDisallowed.push_back( ".mam" );
	VFS::s_vecDisallowed.push_back( ".maq" );
	VFS::s_vecDisallowed.push_back( ".mar" );
	VFS::s_vecDisallowed.push_back( ".mas" );
	VFS::s_vecDisallowed.push_back( ".mat" );
	VFS::s_vecDisallowed.push_back( ".mau" );
	VFS::s_vecDisallowed.push_back( ".mav" );
	VFS::s_vecDisallowed.push_back( ".maw" );
	VFS::s_vecDisallowed.push_back( ".mda" );
	VFS::s_vecDisallowed.push_back( ".mdb" );
	VFS::s_vecDisallowed.push_back( ".mde" );
	VFS::s_vecDisallowed.push_back( ".mdt" );
	VFS::s_vecDisallowed.push_back( ".mdw" );
	VFS::s_vecDisallowed.push_back( ".mdz" );
	VFS::s_vecDisallowed.push_back( ".msc" );
	VFS::s_vecDisallowed.push_back( ".msh" );
	VFS::s_vecDisallowed.push_back( ".msh1" );
	VFS::s_vecDisallowed.push_back( ".msh2" );
	VFS::s_vecDisallowed.push_back( ".mshxml" );
	VFS::s_vecDisallowed.push_back( ".msh1xml" );
	VFS::s_vecDisallowed.push_back( ".msh2xml" );
	VFS::s_vecDisallowed.push_back( ".msi" );
	VFS::s_vecDisallowed.push_back( ".msp" );
	VFS::s_vecDisallowed.push_back( ".mst" );
	VFS::s_vecDisallowed.push_back( ".ops" );
	VFS::s_vecDisallowed.push_back( ".pcd" );
	VFS::s_vecDisallowed.push_back( ".pif" );
	VFS::s_vecDisallowed.push_back( ".plg" );
	VFS::s_vecDisallowed.push_back( ".prf" );
	VFS::s_vecDisallowed.push_back( ".prg" );
	VFS::s_vecDisallowed.push_back( ".pst" );
	VFS::s_vecDisallowed.push_back( ".reg" );
	VFS::s_vecDisallowed.push_back( ".scf" );
	VFS::s_vecDisallowed.push_back( ".scr" );
	VFS::s_vecDisallowed.push_back( ".sct" );
	VFS::s_vecDisallowed.push_back( ".shb" );
	VFS::s_vecDisallowed.push_back( ".shs" );
	VFS::s_vecDisallowed.push_back( ".ps1" );
	VFS::s_vecDisallowed.push_back( ".ps1xml" );
	VFS::s_vecDisallowed.push_back( ".ps2" );
	VFS::s_vecDisallowed.push_back( ".ps2xml" );
	VFS::s_vecDisallowed.push_back( ".psc1" );
	VFS::s_vecDisallowed.push_back( ".psc2" );
	VFS::s_vecDisallowed.push_back( ".tmp" );
	VFS::s_vecDisallowed.push_back( ".url" );
	VFS::s_vecDisallowed.push_back( ".vb" );
	VFS::s_vecDisallowed.push_back( ".vbe" );
	VFS::s_vecDisallowed.push_back( ".vbs" );
	VFS::s_vecDisallowed.push_back( ".vsmacros" );
	VFS::s_vecDisallowed.push_back( ".vsw" );
	VFS::s_vecDisallowed.push_back( ".ws" );
	VFS::s_vecDisallowed.push_back( ".wsc" );
	VFS::s_vecDisallowed.push_back( ".wsf" );
	VFS::s_vecDisallowed.push_back( ".wsh" );
	VFS::s_vecDisallowed.push_back( ".xnk" );



	LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB );



	LUA->CreateTable( );

		// vfs.Open(file, mode)
		LUA->PushCFunction( VFS::Open );
		LUA->SetField( -2, "Open" );

		// vfs.Close(handle)
		LUA->PushCFunction( VFS::Close );
		LUA->SetField( -2, "Close" );

		// vfs.IsValid(handle)
		LUA->PushCFunction( VFS::IsValidFH );
		LUA->SetField( -2, "IsValid" );

		// vfs.Flush(handle)
		LUA->PushCFunction( VFS::Flush );
		LUA->SetField( -2, "Flush" );

		// vfs.Write(handle, write_type, data)
		LUA->PushCFunction( VFS::Write );
		LUA->SetField( -2, "Write" );

		// vfs.Read(handle, read_type)
		LUA->PushCFunction( VFS::Read );
		LUA->SetField( -2, "Read" );

		// vfs.Seek(handle, pos, method)
		LUA->PushCFunction( VFS::Seek );
		LUA->SetField( -2, "Seek" );

		// vfs.Tell(handle)
		LUA->PushCFunction( VFS::Tell );
		LUA->SetField( -2, "Tell" );

		// vfs.EOF(handle)
		LUA->PushCFunction( VFS::IsEOF );
		LUA->SetField( -2, "IsEOF" );

		// vfs.CRC32(handle)
		LUA->PushCFunction( VFS::CRC32 );
		LUA->SetField( -2, "CRC32" );

	LUA->SetField( -2, "vfs" );



	LUA->PushNumber( FILESYSTEM_SEEK_HEAD );
	LUA->SetField( -2, "VFS_SEEK_SET" );

	LUA->PushNumber( FILESYSTEM_SEEK_CURRENT );
	LUA->SetField( -2, "VFS_SEEK_CURRENT" );

	LUA->PushNumber( FILESYSTEM_SEEK_TAIL );
	LUA->SetField( -2, "VFS_SEEK_END" );



	LUA->PushNumber( (double)FS::ReadWrite::data );
	LUA->SetField( -2, "VFS_WRITE_DATA" );

	LUA->PushNumber( (double)FS::ReadWrite::string );
	LUA->SetField( -2, "VFS_WRITE_STRING" );

	LUA->PushNumber( (double)FS::ReadWrite::uint32 );
	LUA->SetField( -2, "VFS_WRITE_UINT32" );

	LUA->PushNumber( (double)FS::ReadWrite::uint16 );
	LUA->SetField( -2, "VFS_WRITE_UINT16" );

	LUA->PushNumber( (double)FS::ReadWrite::uint8 );
	LUA->SetField( -2, "VFS_WRITE_UINT8" );

	LUA->PushNumber( (double)FS::ReadWrite::fp );
	LUA->SetField( -2, "VFS_WRITE_FLOAT" );

	LUA->PushNumber( (double)FS::ReadWrite::doublefp );
	LUA->SetField( -2, "VFS_WRITE_DOUBLE" );

	LUA->PushNumber( (double)FS::ReadWrite::uint8 );
	LUA->SetField( -2, "VFS_WRITE_BYTE" );



	LUA->PushNumber( (double)FS::ReadWrite::data );
	LUA->SetField( -2, "VFS_READ_DATA" );

	LUA->PushNumber( (double)FS::ReadWrite::string );
	LUA->SetField( -2, "VFS_READ_STRING" );

	LUA->PushNumber( (double)FS::ReadWrite::uint32 );
	LUA->SetField( -2, "VFS_READ_UINT32" );

	LUA->PushNumber( (double)FS::ReadWrite::uint16 );
	LUA->SetField( -2, "VFS_READ_UINT16" );

	LUA->PushNumber( (double)FS::ReadWrite::uint8 );
	LUA->SetField( -2, "VFS_READ_UINT8" );

	LUA->PushNumber( (double)FS::ReadWrite::fp );
	LUA->SetField( -2, "VFS_READ_FLOAT" );

	LUA->PushNumber( (double)FS::ReadWrite::doublefp );
	LUA->SetField( -2, "VFS_READ_DOUBLE" );

	LUA->PushNumber( (double)FS::ReadWrite::uint8 );
	LUA->SetField( -2, "VFS_READ_BYTE" );



	LUA->CreateMetaTableType( "IVFS", GarrysMod::Lua::Type::USERDATA );

		LUA->PushCFunction( VFS::GarbageCollect );
		LUA->SetField( -2, "__gc" );
		
		LUA->PushCFunction( VFS::Close );
		LUA->SetField( -2, "Close" );
		
		LUA->PushCFunction( VFS::IsValidFH );
		LUA->SetField( -2, "IsValid" );
		
		LUA->PushCFunction( VFS::Flush );
		LUA->SetField( -2, "Flush" );
		
		LUA->PushCFunction( VFS::Write );
		LUA->SetField( -2, "Write" );
		
		LUA->PushCFunction( VFS::Read );
		LUA->SetField( -2, "Read" );
		
		LUA->PushCFunction( VFS::Seek );
		LUA->SetField( -2, "Seek" );
		
		LUA->PushCFunction( VFS::Tell );
		LUA->SetField( -2, "Tell" );
		
		LUA->PushCFunction( VFS::IsEOF );
		LUA->SetField( -2, "EOF" );
		
		LUA->PushCFunction( VFS::CRC32 );
		LUA->SetField( -2, "CRC32" );
		
		LUA->Push( -1 );
		LUA->SetField( -2, "__index" );

	LUA->Pop( );

	return 0;
}

GMOD_MODULE_CLOSE( )
{
	FS::UnloadFilesystem( );
	return 0;
}