dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"pvs"
	
	INCLUDES	"source_sdk"
	INCLUDES	"gmod_sdk"
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
			--links	"steam_api"
		configuration 		"linux"
