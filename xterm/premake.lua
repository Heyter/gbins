dofile("../common.lua")

SOLUTION"xterm"
	
	INCLUDES	"source_sdk"
	INCLUDES	"gmod_sdk"
	
	configuration 		"linux"
	
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"linux"
