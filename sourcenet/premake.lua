dofile("../common.lua")

-- for some reason it wont compile otherwise
--RequireDefaultlibs()
RequireRuntime()

SOLUTION"sourcenet"
	
	targetdir	"Release"
	INCLUDES	"source_sdk"
	INCLUDES	"hooking"
	INCLUDES	"backwards_headers"
	defines		{"NDEBUG"}
	includedirs	{"src","src/sourcenet","src/engine","src/common"}
	
	WINDOWS()
		defines		{"USE_WINSOCK"}
		links		{"Ws2_32"}
	
	LINUX()
		links{"dl"}
		
	PROJECT()
		files 
		{
			"src/sourcenet/*.cpp",
			"src/engine/*.cpp",
		}
		
		SOURCE_SDK_LINKS()
		configuration 		"windows"
		configuration 		"linux"
			defines "_LINUX"