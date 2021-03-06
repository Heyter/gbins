#include "lua_dyn.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LUA_QL(x)       "'" x "'"
#define LUA_QS          LUA_QL("%s")

#define LUA_PREFIX LuaFunctions.
lua_All_functions LuaFunctions;

#define PROGNAME        "gluac"          /* default program name */
#define OUTPUT          PROGNAME ".luac" /* default output file */

static int dumping=1;                   /* dump bytecodes? */
static char Output[]={ OUTPUT };        /* default output file name */
static const char* output=Output;       /* actual output file name */
static const char* progname=PROGNAME;   /* actual program name */

static void fatal(const char* message)
{
	fprintf(stderr,"%s: %s\n",progname,message);
	exit(EXIT_FAILURE);
}

static void cannot(const char* what)
{
	fprintf(stderr,"%s: cannot %s %s: %s\n",progname,what,output,strerror(errno));
	exit(EXIT_FAILURE);
}

static void usage(const char* message)
{
	if (*message=='-')
		fprintf(stderr,"%s: unrecognized option " LUA_QS "\n",progname,message);
	else
		fprintf(stderr,"%s: %s\n",progname,message);
	fprintf(stderr,
	"usage: %s [options] [filenames].\n"
	"Available options are:\n"
	"  -        process stdin\n"
	"  -o name  output to file " LUA_QL("name") " (default is \"%s\")\n"
	"  -p       parse only\n"
	"  -v       show version information\n"
	"  --       stop handling options\n",
	progname,Output);
	exit(EXIT_FAILURE);
}

#define IS(s) (strcmp(argv[i],s)==0)

static int doargs(int argc, char* argv[])
{
	int i;
	int version=0;
	if (argv[0]!=NULL && *argv[0]!=0) progname=argv[0];
	for (i=1; i<argc; i++)
	{
		if (*argv[i]!='-')                    /* end of options; keep it */
			break;
		else if (IS("--"))                    /* end of options; skip it */
		{
			++i;
			if (version) ++version;
				break;
		}
		else if (IS("-"))                     /* end of options; use stdin */
			break;
		else if (IS("-o"))                    /* output file */
		{
			output=argv[++i];
			if (output==NULL || *output==0) usage(LUA_QL("-o") " needs argument");
			if (IS("-")) output=NULL;
		}
		else if (IS("-p"))                    /* parse only */
			dumping=0;
		else if (IS("-v"))                    /* show version */
			++version;
		else                                  /* unknown option */
			usage(argv[i]);
	}
	if (i==argc && !dumping)
	{
		dumping=0;
		argv[--i]=Output;
	}
	if (version)
	{
		printf("%s  %s\n",LUA_RELEASE,LUA_COPYRIGHT);
		if (version==argc-1) exit(EXIT_SUCCESS);
	}
	return i;
}

struct Smain {
	int argc;
	char** argv;
};

typedef struct {
    size_t *len;
    char **data;
} wdata;

int write_dump(lua_State *L, const void* p, size_t sz, void* ud)
{
	wdata *wd = (wdata *)ud;

	char *newData = (char *)realloc(*(wd->data), (*(wd->len)) + sz);

	if(newData)
	{
		memcpy(newData + (*(wd->len)), p, sz);
		*(wd->data) = newData;
		*(wd->len) += sz;
	} else {
		free(newData);
		return 1;
	}

	return 0;
}

static int pmain(lua_State* L)
{
	struct Smain* s = (struct Smain*)lua_touserdata(L, 1);
	int argc=s->argc;
	char** argv=s->argv;
	int i;

	lua_pop(L,1);

	if (!lua_checkstack(L,argc)) fatal("too many input files");
	for (i=0; i<argc; i++)
	{
		const char* filename=IS("-") ? NULL : argv[i];
		if (luaL_loadfile(L,filename)!=0) fatal(lua_tostring(L,-1));

		if (dumping)
		{
			FILE* D= (output==NULL) ? stdout : fopen(output,(i > 0 ? "ab" : "wb"));
			if (D==NULL) cannot("open");

			char* bytecode = 0L;
			size_t len = 0;
			wdata wd = {&len, &bytecode};

			if(lua_dump(L, write_dump, &wd)) fatal("failed to dump bytecode");

			fwrite(bytecode,len,1,D);

			lua_pop(L,3);

			if (ferror(D)) cannot("write");
			if (fclose(D)) cannot("close");
		}

	}
	return 0;
}

int glua_print(lua_State* L)
{
	int i;
	for (i=1; i <= lua_gettop(L); i++) {
		if (i>1)
			printf("\t");

		lua_getglobal(L, "tostring");
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);

		printf("%s", lua_tostring(L,-1));

		lua_pop(L,1);
	}
	printf("\n");
    return 0;
}

int main(int argc, char* argv[])
{
	#ifdef _WIN32
	HMODULE module = LoadLibrary("lua_shared.dll");
#else
	void* module = dlopen("lua_shared.so", RTLD_LAZY);
#endif
	if(!luaL_loadfunctions(module, &LuaFunctions, sizeof(LuaFunctions)))
	{
		printf("Error loading lua_shared\n");
		return 1;
	}
	
	{
		lua_State* L;
		struct Smain s;
		int i=doargs(argc,argv);
		argc-=i; argv+=i;
		if (argc<=0) usage("no input files given");
		L=lua_open();
		luaL_openlibs(L);
		//lua_register(L, "print", glua_print);
		if (L==NULL) fatal("not enough memory for state");
		s.argc=argc;
		s.argv=argv;
		if (lua_cpcall(L,pmain,&s)!=0) fatal(lua_tostring(L,-1));
		lua_close(L);
	}
	return EXIT_SUCCESS;
}