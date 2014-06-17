#ifndef SOURCENET3_H
#define SOURCENET3_H

#ifdef _WIN32
	#include <winsock.h>
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef GetObject
#endif

#include "ILuaModuleManager.h"

// Enable/disable SendDatagram hooking
extern bool g_bPatchedNetChunk;

// Utility macros

#undef Lua
#define Lua()		pLuaInterface
#define UsesLua()	ILuaInterface *pLuaInterface = modulemanager->GetLuaInterface( L )

#define SOURCENET_META_BASE 5000

#define VerifyStream( stream ) \
	if ( stream < 0 || stream >= MAX_STREAMS ) \
	{ \
		Lua()->PushNil(); \
		return 1; \
	}

#define VerifyOffset( vec, offset ) \
	if ( offset < 0 || offset >= vec.Count() ) \
	{ \
		Lua()->PushNil(); \
		return 1; \
	}

#define CopyUserDataOrNull( arg, meta, dst, dsttype ) \
	int arg___t = Lua()->GetType( arg ); \
	if ( arg___t == GET_META_ID( meta ) ) \
		dst = (dsttype)GET_META( arg, meta ); \
	else if ( arg___t == GLua::TYPE_NUMBER && Lua()->GetInteger( arg ) == 0 ) \
		dst = (dsttype)NULL; \
	else \
		//Lua()->TypeError( GET_META_NAME( meta ), arg )

#define GET_META( index, name )	(name *)Lua()->GetUserData( index )

#define PUSH_META( data, name ) \
	{ \
		if ( data ) \
		{ \
			ILuaObject *META__tbl = Lua()->GetMetaTable( GET_META_NAME( name ), GET_META_ID( name ) ); \
			Lua()->PushUserData( META__tbl, (void *)data); \
			META__tbl->UnReference(); \
		} \
		else \
		{ \
			Lua()->PushNil(); \
		} \
	} \

#define META_FUNCTION( meta, name )		LUA_FUNCTION( meta##__##name )

#define	META_ID( name, id )			const int META_##name##_id = SOURCENET_META_BASE + id; \
						const char *META_##name##_name = #name

#define EXT_META_FUNCTION( meta, name)		extern META_FUNCTION( meta, name )

#define EXT_META_ID( name, id )			extern const int META_##name##_id; \
						extern const char *META_##name##_name

#define GET_META_ID( name )			7 //META_##name##_id
#define GET_META_NAME( name )			META_##name##_name

#define BEGIN_META_REGISTRATION( name ) \
	{ \
		ILuaObject *META__tbl = Lua()->GetMetaTable( GET_META_NAME( name ), GET_META_ID( name ) ); \
		ILuaObject *META__index = Lua()->GetNewTable();

#define REG_META_FUNCTION( meta, name ) 	META__index->SetMember( #name, meta##__##name )
#define REG_META_CALLBACK( meta, name ) 	META__tbl->SetMember( #name, meta##__##name )
#define REG_META_CALLBACK_( meta, name, idx ) 	META__tbl->SetMember( #idx, name )

#define END_META_REGISTRATION( ) \
		META__tbl->SetMember( "__index", META__index ); \
		META__index->UnReference(); \
		META__tbl->UnReference(); \
	} \

#define BEGIN_ENUM_REGISTRATION( name ) \
	{ \
		Lua()->NewGlobalTable( #name ); \
		ILuaObject *ENUM__tbl = Lua()->GetGlobal( #name )

#define REG_ENUM( name, value ) \
	ENUM__tbl->SetMember( #value, (float)value )

#define END_ENUM_REGISTRATION( ) \
		ENUM__tbl->UnReference(); \
	} \

#define GLBL_FUNCTION( name )		LUA_FUNCTION( _G__##name )
#define EXT_GLBL_FUNCTION( name )	extern GLBL_FUNCTION( name )
#define REG_GLBL_FUNCTION( name )	Lua()->SetGlobal( #name, _G__##name )
#define REG_GLBL_NUMBER( name )		Lua()->SetGlobal( #name, (float)name )
#define REG_GLBL_STRING( name )		Lua()->SetGlobal( #name, (const char *)name )

// Multiple Lua state support

#include <utllinkedlist.h>

struct multiStateInfo
{
	lua_State *L;
	int ref_hook_Call;
};

typedef CUtlLinkedList<multiStateInfo> luaStateList_t;

luaStateList_t *GetLuaStates( void );

#define BEGIN_MULTISTATE_HOOK( name ) \
{ \
	luaStateList_t *states = GetLuaStates(); \
	if ( states ) \
	{ \
		for ( int i = 0; i < states->Count(); i++ ) \
		{ \
			multiStateInfo msi = states->Element( i ); \
			lua_State *L = msi.L; \
			UsesLua(); \
			Lua()->PushReference( msi.ref_hook_Call ); \
			Lua()->Push( name ); \
			Lua()->PushNil(); \
			int argc = 0

#define DO_MULTISTATE_HOOK( code ) \
			code; \
			argc++

#define CALL_MULTISTATE_HOOK( returns ) \
			Lua()->Call( 2 + argc, returns )

#define STOP_MULTISTATE_HOOK() \
			break;

#define END_MULTISTATE_HOOK() \
		} \
	} \
	else \
	{ \
		Msg( "GetLuaStates() returned NULL\n" ); \
	} \
}

// Source interfaces

#include <interface.h>

extern CreateInterfaceFn fnEngineFactory;

#ifdef IVENGINESERVER_INTERFACE

#include <eiface.h>

extern IVEngineServer *g_pEngineServer;

#endif

#ifdef IVENGINECLIENT_INTERFACE

#include <cdll_client_int.h>

extern IVEngineClient *g_pEngineClient;

#endif

#ifdef ICVAR_INTERFACE

#include <icvar.h>

extern ICvar *g_pCVarClient;
extern ICvar *g_pCVarServer;

#endif

// Platform definitions

#ifdef _WIN32

#undef CreateEvent

#define BEGIN_MEMEDIT( addr, size ) \
{ \
	DWORD previous; \
	VirtualProtect( addr, \
			size, \
			PAGE_EXECUTE_READWRITE, \
			&previous ); \

#define FINISH_MEMEDIT( addr, size ) \
	VirtualProtect( addr, \
			size, \
			previous, \
			NULL ); \
} \

#elif defined _LINUX

#define strcpy_s(a,b,c) strncpy(a,c,b)

#include <sys/mman.h>
#include <unistd.h>

inline unsigned char *PageAlign( unsigned char *addr, long page )
{
	return addr - ( (DWORD)addr % page );
}

#define BEGIN_MEMEDIT( addr, size ) \
{ \
	long page = sysconf( _SC_PAGESIZE ); \
	mprotect( PageAlign( (unsigned char *)addr, page ), \
			page, \
			PROT_EXEC | PROT_READ | PROT_WRITE );

#define FINISH_MEMEDIT( addr, size ) \
	mprotect( PageAlign( (unsigned char *)addr, page ), \
			page, \
			PROT_EXEC | PROT_READ ); \
}

#endif

#endif // SOURCENET3_H
