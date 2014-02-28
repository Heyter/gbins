dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"enginespew"
	
	targetdir	"Release"
	INCLUDES	"source_sdk"
	INCLUDES	"gmod_sdk"
	defines		{"NDEBUG"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
			--links	"steam_api"
		configuration 		"linux"
