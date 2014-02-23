dofile("../common.lua")

RequireRuntime() -- hack

SOLUTION"enginespew"
	
	-- Flag removing is in BETA!?
	--removeflags "StaticRuntime"
	
	targetdir	"Release"
	INCLUDES	"source_sdk"
	INCLUDES	"gmod_sdk"
	defines		{"NDEBUG"}
	
	WINDOWS()
	
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
		configuration 		"not windows"
