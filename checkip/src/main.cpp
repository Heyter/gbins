#include <GarrysMod/Lua/Interface.h>
#include "LuaInterface.h"
#include <tier1/netadr.h>
#include <symbolfinder.hpp>
#include <detours.h>
#include <cstdint>
#include <iostream>
#include <unordered_set>

#if defined _WIN32

#define MAIN_BINARY_FILE "engine.dll"

#elif defined __linux

#define MAIN_BINARY_FILE "bin/engine_srv.so"
#define SYMBOL_PREFIX "@"

#elif defined __APPLE__

#define MAIN_BINARY_FILE "bin/engine.dylib"
#define SYMBOL_PREFIX "@_"

#endif

typedef bool ( *tFilter_ShouldDiscard )( const netadr_t & );
static tFilter_ShouldDiscard original_Filter_ShouldDiscard = nullptr;
static MologieDetours::Detour<tFilter_ShouldDiscard> *detour_Filter_ShouldDiscard = nullptr;

#if defined _WIN32

#define FILTER_SHOULDDISCARD_SYM reinterpret_cast<const uint8_t *>( "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x8B\x40\x30\x83\xEC\x08\x85\xC0" )
#define FILTER_SHOULDDISCARD_SYMLEN 16

#elif defined __linux || defined __APPLE__

#define FILTER_SHOULDDISCARD_SYM reinterpret_cast<const uint8_t *>( SYMBOL_PREFIX "_Z20Filter_ShouldDiscardRK8netadr_s" )
#define FILTER_SHOULDDISCARD_SYMLEN 0

#endif

typedef void ( *tFilter_SendBan )( const netadr_t & ) ;
static tFilter_SendBan original_Filter_SendBan = nullptr;
static MologieDetours::Detour<tFilter_SendBan> *detour_Filter_SendBan = nullptr;

#if defined _WIN32

#define FILTER_SENDBAN_SYM reinterpret_cast<const uint8_t *>( "\x55\x8B\xEC\x8B\x45\x08\x6A\x6C\x68\x2A\x2A\x2A\x2A\x50\x6A\x01" )
#define FILTER_SENDBAN_SYMLEN 16

#elif defined __linux || defined __APPLE__

#define FILTER_SENDBAN_SYM reinterpret_cast<const uint8_t *>( SYMBOL_PREFIX "_Z14Filter_SendBanRK8netadr_s" )
#define FILTER_SENDBAN_SYMLEN 0

#endif

static std::unordered_set<uint32_t> filter;

static bool hook_Filter_ShouldDiscard( const netadr_t &adr )
{
	return filter.find( *(uint32_t *)&adr.ip[0] ) == filter.end( );
}

static void hook_Filter_SendBan( const netadr_t &adr ) { }

static int EnableFirewallWhitelist( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::BOOL );

	bool hook = LUA->GetBool( 1 );
	if( hook && detour_Filter_ShouldDiscard == nullptr ) 
	{
		bool failed = false;
		try
		{
			detour_Filter_ShouldDiscard = new MologieDetours::Detour<tFilter_ShouldDiscard>( original_Filter_ShouldDiscard, hook_Filter_ShouldDiscard );
		}
		catch( MologieDetours::DetourException &e )
		{
			failed = true;
			LUA->PushString( e.what( ) );
		}

		if( failed )
			LUA->ThrowError( LUA->GetString( -1 ) );
	}
	else if( !hook && detour_Filter_ShouldDiscard != nullptr )
	{
		bool failed = false;
		try
		{
			delete detour_Filter_ShouldDiscard;
			detour_Filter_ShouldDiscard = nullptr;
		}
		catch( MologieDetours::DetourException &e )
		{
			failed = true;
			LUA->PushString( e.what( ) );
		}

		if( failed )
			LUA->ThrowError( LUA->GetString( -1 ) );
	}

	LUA->PushBool( true );
	return 1;
}

static int WhitelistIP( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::NUMBER );
	filter.insert( static_cast<uint32_t>( LUA->GetNumber( 1 ) ) );
	return 0;
}

static int RemoveIP( lua_State *state )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::NUMBER );
	filter.erase( static_cast<uint32_t>( LUA->GetNumber( 1 ) ) );
	return 0;
}

static int WhitelistReset( lua_State *state )
{
	filter.swap( std::unordered_set<uint32_t>( ) );
	LUA->PushBool( true );
	return 1;
}

GMOD_MODULE_OPEN( )
{
	SymbolFinder symfinder;

	original_Filter_SendBan = reinterpret_cast<tFilter_SendBan>( symfinder.ResolveOnBinary( MAIN_BINARY_FILE, FILTER_SENDBAN_SYM, FILTER_SENDBAN_SYMLEN ) );
	if( original_Filter_SendBan != nullptr )
	{
		bool failed = false;
		try
		{
			detour_Filter_SendBan = new MologieDetours::Detour<tFilter_SendBan>( original_Filter_SendBan, hook_Filter_SendBan );
		}
		catch( MologieDetours::DetourException &e )
		{
			failed = true;
			LUA->PushString( e.what( ) );
		}

		if( failed )
			LUA->ThrowError( LUA->GetString( -1 ) );
	}
	else
	{
		LUA->ThrowError( "Filter_SendBan: detour failed, signature not found. (plugin needs updating)\n" );
	}

	original_Filter_ShouldDiscard = reinterpret_cast<tFilter_ShouldDiscard>( symfinder.ResolveOnBinary( MAIN_BINARY_FILE, FILTER_SHOULDDISCARD_SYM, FILTER_SHOULDDISCARD_SYMLEN ) );
	if( original_Filter_ShouldDiscard != nullptr )
	{
		LUA->PushSpecial( GarrysMod::Lua::SPECIAL_GLOB );

		LUA->PushCFunction( EnableFirewallWhitelist );
		LUA->SetField( -2, "EnableFirewallWhitelist" );

		LUA->PushCFunction( WhitelistIP );
		LUA->SetField( -2, "WhitelistIP" );

		LUA->PushCFunction( RemoveIP );
		LUA->SetField( -2, "RemoveIP" );

		LUA->PushCFunction( WhitelistReset );
		LUA->SetField( -2, "WhitelistReset" );
	}
	else
	{
		LUA->ThrowError( "Filter_ShouldDiscard: detour failed, signature not found. (plugin needs updating)\n" );
	}
	
	return 0;
}

GMOD_MODULE_CLOSE( )
{
	try
	{
		if( detour_Filter_ShouldDiscard != nullptr )
			delete detour_Filter_ShouldDiscard;

		if( detour_Filter_SendBan != nullptr )
			delete detour_Filter_SendBan;
	}
	catch( MologieDetours::DetourException &e )
	{
		static_cast<GarrysMod::Lua::ILuaInterface *>( LUA )->Msg( "checkip unload error: %s\n", e.what( ) );
	}

	return 0;
}