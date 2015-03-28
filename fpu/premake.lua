dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"fpu"
	
	INCLUDES	"source_sdk"
	INCLUDES	"gmod_sdk"

	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
		configuration 		"linux"
