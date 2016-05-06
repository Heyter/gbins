
dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"maxminddb"
--	language"C"
	
	--INCLUDES	"gmod_sdk"
	INCLUDES	"lua51"
	defines		{"NDEBUG"}
	includedirs	"libmaxminddb/include"
	includedirs	"libmaxminddb/src"
	includedirs	"libmaxminddb"
	libdirs	"libmaxminddb/src/.libs"
	
	WINDOWS()
		links		{"Ws2_32"}
	
	LINUX()

	PROJECT()


		defines	{"maxminddb_EXPORTS","_CRT_SECURE_NO_WARNINGS"}
		
		files	"lua-maxminddb/maxminddb.c"
		files	"src/main.c"
		files	"libmaxminddb/src/maxminddb.c"
		INCLUDES	"lua51"
		
		configuration 		"windows"
			defines"_WIN32"
		configuration 		"linux"
	
			-- very unorthodox, maybe we shold just let maxminddb compiel by itself
			defines "HAVE_CONFIG_H"
			defines "__USE_POSIX"
		
			buildoptions "-std=c99"
