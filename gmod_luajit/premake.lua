dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"serverplugin_luajit"
	
	targetdir	"Release"
	INCLUDES	"source_sdk"
	INCLUDES	"hooking"			
	INCLUDES	"luajit"
	INCLUDES	"sigscanning"
	defines		{"PRELOADPLUGIN","GMMODULE","GAME_DLL"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		
		INCLUDES	"luajit"
		
		configuration 		"windows"
			--defines 	"__WIN32__"
			files 		"src/detours/*.cpp"
			includedirs "src/detours"
		configuration 		"linux"
			links	{"luajit","dl","rt","m","pthread","c"}
			defines {"_GNU_SOURCE","GNU_SOURCE"}
			--linkoptions		{"-Wl,-rpath='$$ORIGIN'"}
			--linkoptions		{"garrysmod/bin/server_srv.so"}
			--prelinkcommands	{"mkdir -p garrysmod/bin && ln -fs "..SRCDS_DIR.."/garrysmod/bin/server_srv.so garrysmod/bin/server_srv.so "}
			
