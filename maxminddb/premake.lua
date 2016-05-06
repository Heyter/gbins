
dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"maxminddb"
	language"C"
	
	--INCLUDES	"gmod_sdk"
	INCLUDES	"lua51"
	defines		{"NDEBUG"}
	includedirs	"libmaxminddb/include"
	includedirs	"libmaxminddb/src"

	
	WINDOWS()
		links		{"Ws2_32"}
	
	LINUX()

	PROJECT()


		defines	{"maxminddb_EXPORTS","_CRT_SECURE_NO_WARNINGS"}
		
		files{
			"libmaxminddb/src/maxminddb.c",
			"lua-maxminddb/maxminddb.c",
			"src/main.c",
		}
		INCLUDES	"lua51"
		
		configuration 		"windows"
			defines"_WIN32"
		configuration 		"linux"
		