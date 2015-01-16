dofile("../common.lua")

-- for some reason it wont compile otherwise
--RequireDefaultlibs()
RequireRuntime()

SOLUTION"sourcenetinfo"
	
	INCLUDES	"source_sdk"
	INCLUDES	"backwards_headers"
	defines		{"NDEBUG"}
	
	WINDOWS()
	
	LINUX()
		links{"dl"}
		
	PROJECT()
		SOURCE_SDK_LINKS()
		configuration 		"windows"
		configuration 		"linux"
