dofile("../common.lua")

-- for some reason it wont compile otherwise
RequireDefaultlibs()
RequireRuntime()

SOLUTION"stringtables"
	
	INCLUDES	"source_sdk"
	INCLUDES"hooking"
	INCLUDES	"backwards_headers"
	defines		{"NDEBUG"}
	
	WINDOWS()
	
	LINUX()

	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
		configuration 		"linux"
