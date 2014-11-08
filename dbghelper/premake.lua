dofile("../common.lua")

RequireDefaultlibs()

SOLUTION "dbghelper"
	language "C"
	targetdir "Release"
	INCLUDES "lua51"
	defines {"NDEBUG"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		files {"dbghelper.c"}
		INCLUDES "lua51"