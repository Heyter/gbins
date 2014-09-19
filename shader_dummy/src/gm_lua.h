// Missing from lua_shared:

// lua_isuserdata
// lua_tocfunction
// lua_cpcall
// lua_getallocf
// lua_setallocf

// Modified

// lua_typename

#if defined(WIN32) || defined(_WIN32)
#define __WIN32__
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
#define __LINUX__
#endif

#if defined(__APPLE__)
#define __MACOSX__
#endif


#ifdef __WIN32__
	#include "sigscan/sigscan.h"
	#include <windows.h>
#elif defined __LINUX__
	extern "C" {
		#include <dlfcn.h>
	}
	#define PVOID void *
#endif


#if !defined(LUA_VERSION)
struct luaL_Reg;
struct lua_State;
struct lua_Debug;
typedef int (*lua_CFunction) (lua_State *L);
typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);
typedef int (*lua_Writer) (lua_State *L, const void* p, size_t sz, void* ud);
typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);
typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);
typedef double lua_Number;
typedef ptrdiff_t lua_Integer;
#endif

// state manipulation

typedef lua_State*    (*lua_newstate_t) (lua_Alloc f, void *ud);
typedef void          (*lua_close_t) (lua_State *L);
typedef lua_State*    (*lua_newthread_t) (lua_State *L);
typedef lua_CFunction (*lua_atpanic_t) (lua_State *L, lua_CFunction panicf);

//  basic stack manipulation

typedef int   (*lua_gettop_t) (lua_State *L);
typedef void  (*lua_settop_t) (lua_State *L, int idx);
typedef void  (*lua_pushvalue_t) (lua_State *L, int idx);
typedef void  (*lua_remove_t) (lua_State *L, int idx);
typedef void  (*lua_insert_t) (lua_State *L, int idx);
typedef void  (*lua_replace_t) (lua_State *L, int idx);
typedef int   (*lua_checkstack_t) (lua_State *L, int sz);
typedef void  (*lua_xmove_t) (lua_State *from, lua_State *to, int n);

// access functions (stack -> C)

typedef int           (*lua_isnumber_t) (lua_State *L, int idx);
typedef int           (*lua_isstring_t) (lua_State *L, int idx);
typedef int           (*lua_iscfunction_t) (lua_State *L, int idx);
typedef int           (*lua_isuserdata_t) (lua_State *L, int idx);
typedef int           (*lua_type_t) (lua_State *L, int idx);
typedef const char*   (*lua_typename_t) (lua_State *L, int tp);
typedef int           (*lua_equal_t) (lua_State *L, int idx1, int idx2);
typedef int           (*lua_rawequal_t) (lua_State *L, int idx1, int idx2);
typedef int           (*lua_lessthan_t) (lua_State *L, int idx1, int idx2);
typedef lua_Number    (*lua_tonumber_t) (lua_State *L, int idx);
typedef lua_Integer   (*lua_tointeger_t) (lua_State *L, int idx);
typedef int           (*lua_toboolean_t) (lua_State *L, int idx);
typedef const char*   (*lua_tolstring_t) (lua_State *L, int idx, size_t *len);
typedef size_t        (*lua_objlen_t) (lua_State *L, int idx);
typedef lua_CFunction (*lua_tocfunction_t) (lua_State *L, int idx);
typedef void*         (*lua_touserdata_t) (lua_State *L, int idx);
typedef lua_State*    (*lua_tothread_t) (lua_State *L, int idx);
typedef const void*   (*lua_topointer_t) (lua_State *L, int idx);

// push functions (C -> stack)

typedef void  (*lua_pushnil_t) (lua_State *L);
typedef void  (*lua_pushnumber_t) (lua_State *L, lua_Number n);
typedef void  (*lua_pushinteger_t) (lua_State *L, lua_Integer n);
typedef void  (*lua_pushlstring_t) (lua_State *L, const char *s, size_t l);
typedef void  (*lua_pushstring_t) (lua_State *L, const char *s);
typedef const char *(*lua_pushvfstring_t) (lua_State *L, const char *fmt, va_list argp);
typedef const char *(*lua_pushfstring_t) (lua_State *L, const char *fmt, ...);
typedef void  (*lua_pushcclosure_t) (lua_State *L, lua_CFunction fn, int n);
typedef void  (*lua_pushboolean_t) (lua_State *L, int b);
typedef void  (*lua_pushlightuserdata_t) (lua_State *L, void *p);
typedef int   (*lua_pushthread_t) (lua_State *L);

// get functions (Lua -> stack)

typedef void  (*lua_gettable_t) (lua_State *L, int idx);
typedef void  (*lua_getfield_t) (lua_State *L, int idx, const char *k);
typedef void  (*lua_rawget_t) (lua_State *L, int idx);
typedef void  (*lua_rawgeti_t) (lua_State *L, int idx, int n);
typedef void  (*lua_createtable_t) (lua_State *L, int narr, int nrec);
typedef void* (*lua_newuserdata_t) (lua_State *L, size_t sz);
typedef int   (*lua_getmetatable_t) (lua_State *L, int objindex);
typedef void  (*lua_getfenv_t) (lua_State *L, int idx);

//set functions (stack -> Lua)

typedef void  (*lua_settable_t) (lua_State *L, int idx);
typedef void  (*lua_setfield_t) (lua_State *L, int idx, const char *k);
typedef void  (*lua_rawset_t) (lua_State *L, int idx);
typedef void  (*lua_rawseti_t) (lua_State *L, int idx, int n);
typedef int   (*lua_setmetatable_t) (lua_State *L, int objindex);
typedef int   (*lua_setfenv_t) (lua_State *L, int idx);

// `load' and `call' functions (load and run Lua code)

typedef void  (*lua_call_t) (lua_State *L, int nargs, int nresults);
typedef int   (*lua_pcall_t) (lua_State *L, int nargs, int nresults, int errfunc);
typedef int   (*lua_cpcall_t) (lua_State *L, lua_CFunction func, void *ud);
typedef int   (*lua_load_t) (lua_State *L, lua_Reader reader, void *dt, const char *chunkname);
typedef int   (*lua_dump_t) (lua_State *L, lua_Writer writer, void *data);

// coroutine functions

typedef int  (*lua_yield_t) (lua_State *L, int nresults);
typedef int  (*lua_resume_t) (lua_State *L, int narg);
typedef int  (*lua_status_t) (lua_State *L);

// garbage-collection function

typedef int (*lua_gc_t) (lua_State *L, int what, int data);

// miscellaneous functions

typedef int       (*lua_error_t) (lua_State *L);
typedef int       (*lua_next_t) (lua_State *L, int idx);
typedef void      (*lua_concat_t) (lua_State *L, int n);
typedef lua_Alloc (*lua_getallocf_t) (lua_State *L, void **ud);
typedef void      (*lua_setallocf_t) (lua_State *L, lua_Alloc f, void *ud);

// Debug API

typedef int         (*lua_getstack_t) (lua_State *L, int level, lua_Debug *ar);
typedef int         (*lua_getinfo_t) (lua_State *L, const char *what, lua_Debug *ar);
typedef const char* (*lua_getlocal_t) (lua_State *L, const lua_Debug *ar, int n);
typedef const char* (*lua_setlocal_t) (lua_State *L, const lua_Debug *ar, int n);
typedef const char* (*lua_getupvalue_t) (lua_State *L, int funcindex, int n);
typedef const char* (*lua_setupvalue_t) (lua_State *L, int funcindex, int n);
typedef int         (*lua_sethook_t) (lua_State *L, lua_Hook func, int mask, int count);
typedef lua_Hook    (*lua_gethook_t) (lua_State *L);
typedef int         (*lua_gethookmask_t) (lua_State *L);
typedef int         (*lua_gethookcount_t) (lua_State *L);

// Aux API

typedef void        (*luaL_openlib_t) (lua_State *L, const char *libname, const luaL_Reg *l, int nup);
typedef void        (*luaL_register_t) (lua_State *L, const char *libname, const luaL_Reg *l);
typedef int         (*luaL_getmetafield_t) (lua_State *L, int obj, const char *e);
typedef int         (*luaL_callmeta_t) (lua_State *L, int obj, const char *e);
typedef int         (*luaL_typerror_t) (lua_State *L, int narg, const char *tname);
typedef int         (*luaL_argerror_t) (lua_State *L, int numarg, const char *extramsg);
typedef const char* (*luaL_checklstring_t) (lua_State *L, int numArg, size_t *l);
typedef const char* (*luaL_optlstring_t) (lua_State *L, int numArg, const char *def, size_t *l);
typedef lua_Number  (*luaL_checknumber_t) (lua_State *L, int numArg);
typedef lua_Number  (*luaL_optnumber_t) (lua_State *L, int nArg, lua_Number def);
typedef lua_Integer (*luaL_checkinteger_t) (lua_State *L, int numArg);
typedef lua_Integer (*luaL_optinteger_t) (lua_State *L, int nArg, lua_Integer def);
typedef void        (*luaL_checkstack_t) (lua_State *L, int sz, const char *msg);
typedef void        (*luaL_checktype_t) (lua_State *L, int narg, int t);
typedef void        (*luaL_checkany_t) (lua_State *L, int narg);
typedef int         (*luaL_newmetatable_t) (lua_State *L, const char *tname);
typedef void*       (*luaL_checkudata_t) (lua_State *L, int ud, const char *tname);
typedef void        (*luaL_where_t) (lua_State *L, int lvl);
typedef int         (*luaL_error_t) (lua_State *L, const char *fmt, ...);
typedef int         (*luaL_checkoption_t) (lua_State *L, int narg, const char *def, const char *const lst[]);
typedef int         (*luaL_ref_t) (lua_State *L, int t);
typedef void        (*luaL_unref_t) (lua_State *L, int t, int ref);
typedef int         (*luaL_loadfile_t) (lua_State *L, const char *filename);
typedef int         (*luaL_loadbuffer_t) (lua_State *L, const char *buff, size_t sz, const char *name);
typedef int         (*luaL_loadstring_t) (lua_State *L, const char *s);
typedef lua_State*  (*luaL_newstate_t) (void);
typedef const char* (*luaL_gsub_t) (lua_State *L, const char *s, const char *p, const char *r);
typedef const char* (*luaL_findtable_t) (lua_State *L, int idx,const char *fname, int szhint);

// Lua libs

typedef void		(*luaL_openlibs_t) (lua_State *L);

// LuaJIT

typedef int			(*luaJIT_setmode_t)  (lua_State *L, int idx, int mode);

// Garry functions

typedef int			(*luaL_newmetatable_type_t) (lua_State *L, const char tname, int typenum);

#define FUNCLIST(x) \
	x(lua_newstate) \
	x(lua_close) \
	x(lua_newthread) \
	x(lua_atpanic) \
	\
	x(lua_gettop) \
	x(lua_settop) \
	x(lua_pushvalue) \
	x(lua_remove) \
	x(lua_insert) \
	x(lua_replace) \
	x(lua_checkstack) \
	x(lua_xmove) \
	\
	x(lua_isnumber) \
	x(lua_isstring) \
	x(lua_iscfunction) \
	x(lua_isuserdata) \
	x(lua_type) \
	/*x(lua_typename)*/ \
	x(lua_equal) \
	x(lua_rawequal) \
	x(lua_lessthan) \
	x(lua_tonumber) \
	x(lua_tointeger) \
	x(lua_toboolean) \
	x(lua_tolstring) \
	x(lua_objlen) \
	x(lua_tocfunction) \
	x(lua_touserdata) \
	x(lua_tothread) \
	x(lua_topointer) \
	\
	x(lua_pushnil) \
	x(lua_pushnumber) \
	x(lua_pushinteger) \
	x(lua_pushlstring) \
	x(lua_pushstring) \
	x(lua_pushvfstring) \
	x(lua_pushfstring) \
	x(lua_pushcclosure) \
	x(lua_pushboolean) \
	x(lua_pushlightuserdata) \
	x(lua_pushthread) \
	\
	x(lua_gettable) \
	x(lua_getfield) \
	x(lua_rawget) \
	x(lua_rawgeti) \
	x(lua_createtable) \
	x(lua_newuserdata) \
	x(lua_getmetatable) \
	x(lua_getfenv) \
	\
	x(lua_settable) \
	x(lua_setfield) \
	x(lua_rawset) \
	x(lua_rawseti) \
	x(lua_setmetatable) \
	x(lua_setfenv) \
	\
	x(lua_call) \
	x(lua_pcall) \
	x(lua_cpcall) \
	x(lua_load) \
	x(lua_dump) \
	\
	x(lua_yield) \
	/*x(lua_resume)*/ \
	x(lua_status) \
	\
	x(lua_gc) \
	\
	x(lua_error) \
	x(lua_next) \
	x(lua_concat) \
	x(lua_getallocf) \
	x(lua_setallocf) \
	\
	x(lua_getstack) \
	x(lua_getinfo) \
	x(lua_getlocal) \
	x(lua_setlocal) \
	x(lua_getupvalue) \
	x(lua_setupvalue) \
	x(lua_sethook) \
	x(lua_gethook) \
	x(lua_gethookmask) \
	x(lua_gethookcount) \
	\
	x(luaL_openlib) \
	x(luaL_register) \
	x(luaL_getmetafield) \
	x(luaL_callmeta) \
	x(luaL_typerror) \
	x(luaL_argerror) \
	x(luaL_checklstring) \
	x(luaL_optlstring) \
	x(luaL_checknumber) \
	x(luaL_optnumber) \
	x(luaL_checkinteger) \
	x(luaL_optinteger) \
	x(luaL_checkstack) \
	x(luaL_checktype) \
	x(luaL_checkany) \
	x(luaL_newmetatable) \
	x(luaL_checkudata) \
	x(luaL_where) \
	x(luaL_error) \
	x(luaL_checkoption) \
	x(luaL_ref) \
	x(luaL_unref) \
	x(luaL_loadfile) \
	x(luaL_loadbuffer) \
	x(luaL_loadstring) \
	x(luaL_newstate) \
	x(luaL_gsub) \
	x(luaL_findtable) \
	\
	x(luaL_openlibs) \
	\
	x(luaJIT_setmode) \
	\
	x(luaL_newmetatable_type)

#define EXTERN_FUNCS(x) extern x##_t g##x;

FUNCLIST(EXTERN_FUNCS)
extern lua_resume_t glua_resume;

void InitLuaAPI();
void ShutdownLuaAPI();
void DebugLuaAPI();
