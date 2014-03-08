dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"gameserver"
	
	targetdir	"Release"
	INCLUDES	"steamworks"
	
	INCLUDES	"gmod_sdk"
	INCLUDES	"source_sdk"
	
	defines		{"NDEBUG"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
			links	"steam_api"
		configuration 		"linux"
