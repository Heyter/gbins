dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"navigation"
	
	INCLUDES	"source_sdk"
	INCLUDES	"backwards_headers"
	defines		{"NDEBUG"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
			--links	"steam_api"
		configuration 		"linux"
