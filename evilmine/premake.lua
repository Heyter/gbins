dofile("../common.lua")

RequireDefaultlibs()

SOLUTION"evilmine"
	
	INCLUDES	"sigscanning"
	INCLUDES	"lua51"
	INCLUDES	"source_sdk"
	INCLUDES	"hooking"
	--INCLUDES	"steamworks"
	INCLUDES	"gmod_sdk"
	defines		{"NDEBUG"}
	
	WINDOWS()
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		INCLUDES	"lua51"
		INCLUDES	"hooking"
		INCLUDES	"sigscanning"
		configuration 		"windows"
		configuration 		"linux"
		buildoptions"-fpermissive"