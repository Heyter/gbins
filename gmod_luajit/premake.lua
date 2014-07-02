dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"serverplugin_luajit"
	
	targetdir	"Release"
	INCLUDES	"source_sdk"
	INCLUDES	"hooking"
	INCLUDES	"sigscanning"
	defines		{"SERVERPLUGIN","GMMODULE","GAME_DLL"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
			--defines 	"__WIN32__"
			files 		"src/detours/*.cpp"
			includedirs "src/detours"
		configuration 		"linux"
			--defines "__LINUX__"
			--linkoptions		{"-Wl,-rpath='$$ORIGIN'"}
			--linkoptions		{"garrysmod/bin/server_srv.so"}
			--prelinkcommands	{"mkdir -p garrysmod/bin && ln -fs "..SRCDS_DIR.."/garrysmod/bin/server_srv.so garrysmod/bin/server_srv.so "}
			
