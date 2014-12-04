#include <GarrysMod/Lua/Interface.h>
#include <tier1/netadr.h>
#include <detours.h>
#include <iostream>
#include <unordered_set>

#include "SymbolFinder.hpp"

#if !defined LUA_FUNCTION

#define LUA_FUNCTION( name ) int name( lua_State *state )
#define LUA_FUNCTION_STATIC( name ) static LUA_FUNCTION( name )

#endif

#if defined _WIN32

#define MAIN_BINARY_FILE "bin/engine.dylib"

#elif defined __linux

#define MAIN_BINARY_FILE "bin/engine_srv.so"
#define SYMBOL_PREFIX "@"

#elif defined __APPLE__

#define MAIN_BINARY_FILE "bin/engine.dylib"
#define SYMBOL_PREFIX "@_"

#endif

typedef bool ( *tShouldDiscard )( const netadr_t & );
static tShouldDiscard original_ShouldDiscard = nullptr;
static MologieDetours::Detour<tShouldDiscard> *detour_ShouldDiscard = nullptr;

#if defined _WIN32

#define SHOULD_DISCARD_SYM reinterpret_cast<const uint8_t *>( "" )
#define SHOULD_DISCARD_SYMLEN 0

#elif defined __linux || defined __APPLE__

#define SHOULD_DISCARD_SYM reinterpret_cast<const uint8_t *>( SYMBOL_PREFIX "_Z20Filter_ShouldDiscardRK8netadr_s" )
#define SHOULD_DISCARD_SYMLEN 0

#endif

typedef void ( *tFilter_SendBan )( const netadr_t & ) ;
static tFilter_SendBan original_Filter_SendBan = nullptr;
static MologieDetours::Detour<tFilter_SendBan> *detour_Filter_SendBan = nullptr;

#if defined _WIN32

#define FILTER_SENDBAN_SYM reinterpret_cast<const uint8_t *>( "" )
#define FILTER_SENDBAN_SYMLEN 0

#elif defined __linux || defined __APPLE__

#define FILTER_SENDBAN_SYM reinterpret_cast<const uint8_t *>( SYMBOL_PREFIX "_Z14Filter_SendBanRK8netadr_s" )
#define FILTER_SENDBAN_SYMLEN 0

#endif

static std::unordered_set<uint32_t> filter;

static void LOG( const char *x )
{
	std::cerr << x;
}

static bool hook_ShouldDiscard( const netadr_t &adr )
{
	return filter.find( *(uint32_t *)&adr.ip[0] ) != filter.end( );
}

static void hook_Filter_SendBan( const netadr_t &adr ) { }

LUA_FUNCTION_STATIC( EnableFirewallWhitelist )
{
	bool hook = LUA->GetBool( 1 );
	LUA->Pop( 1 );

	if( hook && detour_ShouldDiscard == nullptr ) 
	{
		try
		{
			detour_ShouldDiscard = new MologieDetours::Detour<tShouldDiscard>( original_ShouldDiscard, hook_ShouldDiscard );
		}
		catch( MologieDetours::DetourException &e )
		{
			LOG( "ShouldDiscard: Detour failed: Internal error?\n" );
			return 0;
		}
	}
	else if( !hook && detour_ShouldDiscard != nullptr )
	{
		delete detour_ShouldDiscard;
		detour_ShouldDiscard = nullptr;
	}

	LUA->PushBool( true );
	return 1;
}

LUA_FUNCTION_STATIC( WhitelistIP )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::NUMBER );
	filter.insert( static_cast<uint32_t>( LUA->GetNumber( 1 ) ) );
	return 0;
}

LUA_FUNCTION_STATIC( RemoveIP )
{
	LUA->CheckType( 1, GarrysMod::Lua::Type::NUMBER );
	filter.erase( static_cast<uint32_t>( LUA->GetNumber( 1 ) ) );
	return 0;
}

LUA_FUNCTION_STATIC( WhitelistReset )
{
	filter.swap( std::unordered_set<uint32_t>( ) );
	LUA->PushBool( true );
	return 1;
}

GMOD_MODULE_OPEN( )
{
	SymbolFinder symfinder;

	original_ShouldDiscard = reinterpret_cast<tShouldDiscard>( symfinder.ResolveOnBinary( MAIN_BINARY_FILE, SHOULD_DISCARD_SYM, SHOULD_DISCARD_SYMLEN ) );
	if( original_ShouldDiscard != nullptr )
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
		LOG( "ShouldDiscard: Detour failed: Signature not found. (plugin needs updating)\n" );
	}

	original_Filter_SendBan = reinterpret_cast<tFilter_SendBan>( symfinder.ResolveOnBinary( MAIN_BINARY_FILE, FILTER_SENDBAN_SYM, FILTER_SENDBAN_SYMLEN ) );
	if( original_Filter_SendBan != nullptr )
	{
		try
		{
			detour_Filter_SendBan = new MologieDetours::Detour<tFilter_SendBan>(original_Filter_SendBan, hook_Filter_SendBan);
		}
		catch( MologieDetours::DetourException &e )
		{
			LOG("Filter_SendBan: Detour failed: Internal error?\n");
		}
	}
	else
	{
		LOG("Filter_SendBan: Detour failed: Signature not found. (plugin needs updating)\n");
	}
	
	return 0;
}

GMOD_MODULE_CLOSE( )
{
	if( detour_ShouldDiscard != nullptr )
		delete detour_ShouldDiscard;

	if( detour_Filter_SendBan != nullptr )
		delete detour_Filter_SendBan;

	return 0;
}