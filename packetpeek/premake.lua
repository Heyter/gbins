dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"packetpeek"
	
	INCLUDES	"source_sdk"
	INCLUDES	"gmod_sdk"
	
	configuration 		"linux"
		INCLUDES	"hooking"
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
			--links	"steam_api"
		configuration 		"linux"
			INCLUDES	"hooking"
