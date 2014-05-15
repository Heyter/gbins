dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"ragdoll"
	
	targetdir	"Release"
	INCLUDES	"source_sdk"
	INCLUDES	"gmod_sdk"
	defines		{"NDEBUG","GMMODULE","GAME_DLL"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
		configuration 		"linux"
